#include "Scene_base.h"

#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <sstream>

#include <Component/CameraInput.h>
#include <Component/Transform/Transform.h>

#include "Scene_input.h"

#include <Core/Engine.h>
#include <migine/game_objects/components/Transform.h>

#include <migine/Resource_manager.h>
#include <migine/contact_detection/Collider_base.h>
#include <migine/physics/Contact_resolver.h>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::min;
using std::stringstream;
using std::pair;
using std::vector;
using std::array;

using glm::quat;
using glm::vec3;
using glm::mat3;
using glm::conjugate;
using glm::cross;
using glm::dot;
using glm::length2;

using gsl::not_null;

namespace migine {

	Scene_base::Scene_base() {
		init_resources();
	}

	Scene_base::~Scene_base() {
	}

	void Scene_base::register_game_object2(unique_ptr<Game_object> game_object) {
		if (Collider_base* collider_ptr = dynamic_cast<Collider_base*>(game_object.get()); collider_ptr) {
			bvh.cache_contacts_and_insert(collider_ptr);
			//colliders.push_back(collider_ptr);
		}
		if (Renderer_base* renderer_ptr = dynamic_cast<Renderer_base*>(game_object.get()); renderer_ptr) {
			renderers.push_back(renderer_ptr);
		}
		if (Rigid_body* rigid_body = dynamic_cast<Rigid_body*>(game_object.get()); rigid_body) {
			//rigid_bodies.push_back(rigid_body);
			// TODO add gravity and drag based on class (gl with that)
		}
		game_objects.push_back(move(game_object));
	}

	void Scene_base::init_resources() {
		// sets common GL states
		glClearColor(0, 0, 0, 1);

		draw_ground_plane = true;

		object_model = new EngineComponents::Transform();

		camera = make_unique<Camera>();
		camera->SetPerspective(60, window->props.aspectRatio, 0.01f, 200);
		camera->transform->SetMoveSpeed(2);
		//camera->transform->SetWorldPosition(vec3(0, 5.8f, 20));
		//camera->transform->SetWorldRotation(vec3(-15, 0, 0));
		camera->transform->SetWorldPosition(vec3(0, 1.8f, 0));
		camera->transform->SetWorldRotation(vec3(-15, 0, 0));
		camera->Update();

		camera_input = make_unique<CameraInput>(camera.get());
		window = Engine::GetWindow();

		Scene_input* SI = new Scene_input(*this);

		xoz_plane = new Mesh(Mesh_id::plane);
		xoz_plane->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");

		Resource_manager& rm = get_resource_manager();

		{
			vector<VertexFormat> vertices =
			{
				VertexFormat(vec3(0, 0, 0), vec3(0, 1, 0)),
				VertexFormat(vec3(0, 1, 0), vec3(0, 1, 0)),
			};
			vector<unsigned int> indices = {0, 1};

			auto tmp = make_unique<Mesh>(Mesh_id::line);
			simple_line = tmp.get();
			simple_line->InitFromData(vertices, indices);
			simple_line->SetDrawMode(GL_LINES);
			rm.store_mesh(move(tmp));
		}

		// Create a shader program for drawing face polygon with the color of the normal
		{
			auto shader = make_unique<Shader>(Shader_id::simple);
			shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
			shader->AddShader(RESOURCE_PATH::SHADERS + "Default.FS.glsl", GL_FRAGMENT_SHADER);
			shader->CreateAndLink();
			rm.store_shader(move(shader));
		}

		// Create a shader program for drawing vertex colors
		{
			auto shader = make_unique<Shader>(Shader_id::color);
			shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
			shader->AddShader(RESOURCE_PATH::SHADERS + "Color.FS.glsl", GL_FRAGMENT_SHADER);
			shader->CreateAndLink();
			rm.store_shader(move(shader));
		}

		// Create a shader program for drawing face polygon with the color of the normal
		{
			auto shader = make_unique<Shader>(Shader_id::vertex_normal);
			shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
			shader->AddShader(RESOURCE_PATH::SHADERS + "Normals.FS.glsl", GL_FRAGMENT_SHADER);
			shader->CreateAndLink();
			rm.store_shader(move(shader));
		}

		// Create a shader program for drawing vertex colors
		{
			auto shader = make_unique<Shader>(Shader_id::vertex_color);
			shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
			shader->AddShader(RESOURCE_PATH::SHADERS + "VertexColor.FS.glsl", GL_FRAGMENT_SHADER);
			shader->CreateAndLink();
			rm.store_shader(move(shader));
		}

		// Default rendering mode will use depth buffer
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}

	void Scene_base::frame_start() {
		// clears the color buffer (using the previously set color) and depth buffer
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::ivec2 resolution = window->GetResolution();
		// sets the screen area where to draw
		glViewport(0, 0, resolution.x, resolution.y);
	}

	void Scene_base::update(float delta_time_seconds) {
		// cap delta time
		float caped_delta_time = min(delta_time_seconds, 1.0f / 20);

		// apply forces
		force_registry.update_forces(caped_delta_time);

		// move bodies and update colliders in bvh (aka broad worst_collision phase)
		for (auto& bc_pair : bodies_and_colliders) {
			auto& rigid_body = get<not_null<Rigid_body*>>(bc_pair);
			bool has_moved = rigid_body->integrate(caped_delta_time);
			if (has_moved) {
				auto collider = get<Collider_base*>(bc_pair);
				if (collider) {
					bvh.update(collider);
				}
			}
		}

		// repair bvh
		bvh.clean_dirty_nodes();

		// narrow worst_collision phase
		int pairs_in_contact = 0;
		vector<unique_ptr<Contact>> contacts;
		for (auto& [obj0, obj1] : bvh.get_contacts()) {
			auto new_collisions = obj0->check_collision(*obj1);
			if (new_collisions.size() > 0) {
				pairs_in_contact++;
			}
			for (auto& new_collision : new_collisions) {
				contacts.push_back(move(new_collision));
			}
		}

		// resolve contacts
		if (!contacts.empty()) {
			// prepare resolver
			Contact_resolver contact_resolver(contacts, caped_delta_time); //DEMO1

			// solve contacts
			contact_resolver.resolve_contacts(contacts); // DEMO1
		}

		// render
		for (auto& renderer : renderers) {
			renderer->render(this->get_scene_camera());
		}
		bvh.render_all(*camera);

		// print stats
		static float last_printing_time = 0;
		static int total_frames = 0;
		static int frames_since_printing = 0;
		frames_since_printing++;
		total_frames++;
		float current_time = get_elapsed_time();
		if (float delta_time_printing = current_time - last_printing_time; delta_time_printing > 0.66) {
			stringstream ss;
			ss << "fps:" << frames_since_printing / delta_time_printing << ";";
			frames_since_printing = 0;
			last_printing_time = current_time;
			ss << " broad contacts:" << bvh.get_contact_count() << ";";
			ss << " pairs in narrow contact:" << pairs_in_contact << ";";
			ss << " insertions:" << bvh.insertion_count << ";";
			//ss << " broad intersection checks:" << bvh.aabb_intersection_operations_count << ";";
			ss << " time:" << current_time << ";";
			ss << " frames:" << total_frames << ";";
			continous_print_line_reset();
			continous_print(ss.str());
		}
	}

	void Scene_base::frame_end() {
	}

	void Scene_base::draw_coordinat_system() {
		draw_coordinat_system(camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}

	void Scene_base::draw_coordinat_system(const glm::mat4& view_matrix, const glm::mat4& projection_maxtix) {
		Resource_manager& rm = get_resource_manager();

		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		 // Render the coordinate system
		{
			const Shader& shader = rm.get_shader(Shader_id::color);
			shader.Use();
			glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_maxtix));

			if (draw_ground_plane) {
				object_model->SetScale(glm::vec3(1));
				object_model->SetWorldPosition(glm::vec3(0));
				glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
				glUniform3f(shader.GetUniformLocation("color"), 0.5f, 0.5f, 0.5f);
				xoz_plane->Render();
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glLineWidth(3);
			object_model->SetScale(glm::vec3(1, 25, 1));
			object_model->SetWorldRotation(glm::quat());
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 0, 1, 0);
			simple_line->Render();

			object_model->SetWorldRotation(glm::vec3(0, 0, -90));
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 1, 0, 0);
			simple_line->Render();

			object_model->SetWorldRotation(glm::vec3(90, 0, 0));
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 0, 0, 1);
			simple_line->Render();

			object_model->SetWorldRotation(glm::quat());

			glLineWidth(1);
		}
	}

	void Scene_base::render_mesh(const Mesh& mesh, const Shader& shader, glm::vec3 position, glm::vec3 scale) {
		if (!shader.program) {
			return;
		}

		// render an object using the specified shader and the specified position
		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glm::mat4 model(1);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
		mesh.Render();
	}

	void Scene_base::render_mesh(const Mesh& mesh, glm::vec3 position, glm::vec3 scale) {
		Resource_manager& rm = get_resource_manager();
		render_mesh(mesh, rm.get_shader(Shader_id::simple), position, scale);
	}

	void Scene_base::render_mesh_2d(const Mesh& mesh, const Shader& shader, const glm::mat3& model_matrix) {
		if (!shader.program) {
			return;
		}

		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glm::mat3 mm = model_matrix;
		glm::mat4 model = glm::mat4(
			mm[0][0], mm[0][1], mm[0][2], 0.f,
			mm[1][0], mm[1][1], mm[1][2], 0.f,
			0.f, 0.f, mm[2][2], 0.f,
			mm[2][0], mm[2][1], 0.f, 1.f);

		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
		mesh.Render();
	}

	void Scene_base::render_mesh_2d(const Mesh& mesh, const glm::mat3& model_matrix, const glm::vec3 color) const {
		Resource_manager& rm = get_resource_manager();
		const Shader& shader = rm.get_shader(Shader_id::color);

		if (!shader.program) {
			return;
		}

		glm::mat3 mm = model_matrix;
		glm::mat4 model = glm::mat4(
			mm[0][0], mm[0][1], mm[0][2], 0.f,
			mm[1][0], mm[1][1], mm[1][2], 0.f,
			0.f, 0.f, mm[2][2], 0.f,
			mm[2][0], mm[2][1], 0.f, 1.f);

		// render an object using the specified shader and the specified position
		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(shader.GetUniformLocation("color"), color.r, color.g, color.b);

		mesh.Render();
	}

	void Scene_base::render_mesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model_matrix) {
		if (!shader.program) {
			return;
		}

		// render an object using the specified shader and the specified position
		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));

		mesh.Render();
	}

	Camera& Scene_base::get_scene_camera() const {
		return *camera;
	}

	std::vector<std::unique_ptr<Force_generator_base>>& Scene_base::get_initial_force_generators(gsl::not_null<Rigid_body*> r_body) {
		auto& r_body_default_fs_gen = r_body->get_default_fs_gen();
		if (r_body_default_fs_gen.size() != 0) {
			return r_body_default_fs_gen;
		}
		return this->default_fs_gen;
	}

	InputController& Scene_base::get_camera_input() const {
		return *camera_input;
	}
}