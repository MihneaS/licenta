#include "Scene_base.h"

#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>

#include <Component/CameraInput.h>
#include <Component/Transform/Transform.h>

#include "Scene_input.h"

#include <Core/Engine.h>
#include <migine/Game_objects/Components/Transform.h>

#include <migine/Resource_manager.h>
#include <migine/Game_objects/Contact_detection/Collider_base.h>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::move;

namespace migine {

	Scene_base::Scene_base() {
		init_resources();
	}

	Scene_base::~Scene_base() {
	}

	void Scene_base::register_game_object(unique_ptr<Game_object> game_object) {
		if (Collider_base* collider_ptr = dynamic_cast<Collider_base*>(game_object.get()); collider_ptr) {
			bvh.cache_contacts_and_insert(collider_ptr);
			colliders.push_back(collider_ptr);
		}
		if (Renderer_base* renderer_ptr = dynamic_cast<Renderer_base*>(game_object.get()); renderer_ptr) {
			renderers.push_back(renderer_ptr);
		}
		if (Rigid_body* rigid_body = dynamic_cast<Rigid_body*>(game_object.get()); rigid_body) {
			rigid_bodies.push_back(rigid_body);
		}
		game_objects.push_back(move(game_object));
	}

	void Scene_base::init_resources() {
		// sets common GL states
		glClearColor(0, 0, 0, 1);

		draw_ground_plane = true;

		object_model = new EngineComponents::Transform();

		camera = new Camera();
		camera->SetPerspective(60, window->props.aspectRatio, 0.01f, 200);
		camera->transform->SetMoveSpeed(2);
		camera->transform->SetWorldPosition(glm::vec3(0, 1.6f, 2.5));
		camera->transform->SetWorldRotation(glm::vec3(-15, 0, 0));
		camera->Update();

		camera_input = new CameraInput(camera);
		window = Engine::GetWindow();

		Scene_input* SI = new Scene_input(*this);

		xoz_plane = new Mesh(Mesh_id::plane);
		xoz_plane->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");

		Resource_manager& rm = get_resource_manager();

		{
			vector<VertexFormat> vertices =
			{
				VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)),
				VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
			};
			std::vector<unsigned int> indices = {0, 1};

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

	InputController& Scene_base::get_camera_input() const {
		return *camera_input;
	}
}