#include "Scene_base.h"

#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <sstream>
#include <random>

#include <Component/CameraInput.h>
#include <Component/Transform/Transform.h>

#include "Scene_input.h"

#include <Core/Engine.h>
#include <migine/game_objects/components/Transform.h>

#include <migine/Resource_manager.h>
#include <migine/contact_detection/Collider_base.h>
#include <migine/physics/Contact_resolver.h>
#include <migine/game_objects/shapes/Wall.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/game_objects/shapes/Box.h>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::min;
using std::stringstream;
using std::pair;
using std::vector;
using std::array;
using std::unordered_set;
using std::bind;
using std::default_random_engine;
using std::uniform_real_distribution;

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

	void Scene_base::basic_bool_button_changer(int key, int mods) {
		switch (key) {
		case GLFW_KEY_T:
			time_stopped = !time_stopped;
			break;
		case GLFW_KEY_Y:
			time_slowed = !time_slowed;
			break;
		case GLFW_KEY_P:
			do_resolve_penetrations = !do_resolve_penetrations;
			break;
		case GLFW_KEY_V:
			do_resolve_velocities = !do_resolve_velocities;
			break;
		case GLFW_KEY_1:
			penetration_type = 1;
			break;
		case GLFW_KEY_2:
			penetration_type = 2;
			break;
		case GLFW_KEY_3:
			penetration_type = 3;
			break;
		case GLFW_KEY_X:
			see_bvh = !see_bvh;
			break;
		case GLFW_KEY_Z:
			see_walls = !see_walls;
			break;
		case GLFW_KEY_RIGHT_BRACKET:
			run_one_frame = true;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			create_and_shoot_ball();
			break;
		}
	}

	void Scene_base::modify_contacts(vector<unique_ptr<Contact>>& contacts) {
	}

	const unordered_set<not_null<Collider_base*>> Scene_base::get_objects_in_contact_with(not_null<Collider_base*> collider) const {
		return bvh.get_objects_in_contact_with(collider);
	}

	void Scene_base::spawn_walls(vec3 pos_down_center, vec3 enclosed_volume_scale) {
		constexpr float wall_thickness = 20;
		constexpr float half_thickness = wall_thickness / 2;
		vec3 half_scale = enclosed_volume_scale / 2.0f;

		{ // wall below
			vec3 wall_below_pos = vec3{0, -half_thickness, 0};
			vec3 wall_below_scale = vec3{enclosed_volume_scale.x + 2 * wall_thickness,
										 wall_thickness,
										 enclosed_volume_scale.z + 2 * wall_thickness};
			auto wall_below = make_unique<Wall>(wall_below_pos, wall_below_scale, quat(), k_default_wall_color - vec3{0.05, 0.05, 0});
			set_name(wall_below.get(), "pamant");
			register_game_object(move(wall_below));
		}

		{ // wall left
			vec3 wall_left_pos = vec3{-(half_scale.x + half_thickness), half_scale.y, 0};
			vec3 wall_left_scale = vec3{wall_thickness, enclosed_volume_scale.y, enclosed_volume_scale.z + 2 * wall_thickness};
			auto wall_left = make_unique<Wall>(wall_left_pos, wall_left_scale);
			set_name(wall_left.get(), "wall_left");
			register_game_object(move(wall_left));
		}

		{ // wall right
			vec3 wall_right_pos = vec3{half_scale.x + half_thickness, half_scale.y, 0};
			vec3 wall_right_scale = vec3{wall_thickness, enclosed_volume_scale.y, enclosed_volume_scale.z + 2 * wall_thickness};
			auto wall_right = make_unique<Wall>(wall_right_pos, wall_right_scale);
			set_name(wall_right.get(), "wall_right");
			register_game_object(move(wall_right));
		}

		{ // wall front
			vec3 wall_front_pos = vec3{0, half_scale.y, -(half_scale.z + half_thickness)};
			vec3 wall_front_scale = vec3{enclosed_volume_scale.x, enclosed_volume_scale.y, wall_thickness};
			auto wall_front = make_unique<Wall>(wall_front_pos, wall_front_scale);
			set_name(wall_front.get(), "wall_front");
			register_game_object(move(wall_front));
		}

		{ // wall back
			vec3 wall_back_pos = vec3{0, half_scale.y, half_scale.z + half_thickness};
			vec3 wall_back_scale = vec3{enclosed_volume_scale.x, enclosed_volume_scale.y, wall_thickness};
			auto wall_back = make_unique<Wall>(wall_back_pos, wall_back_scale);
			set_name(wall_back.get(), "wall_back");
			register_game_object(move(wall_back));
		}
	}

	void Scene_base::spawn_walls_large() {
		spawn_walls(vec3{0}, {200, 20, 200});
	}

	void Scene_base::spawn_walls_small() {
		spawn_walls(vec3{0}, {10, 5, 10});
	}


	void Scene_base::spawn_balls(int n) {
		// build random generatior with the same seed in order to have the same output at every run
		auto rand = bind(uniform_real_distribution<float>{0, 1}, default_random_engine{42});
		static int ball_counter = 0;
		constexpr float init_r = 5;
		constexpr float r_step = 2;
		constexpr float max_initial_speed = 10; // 100;
		float r = init_r;
		for (int i = 0; i < n / 10; i++, r += r_step) {
			int j_limit = i == n / 10 ? n % 10 : 10;
			vec3 axis = generate_point_on_sphere(rand);
			quat q_axis = glm::rotate(quat(), axis);
			for (int j = 0; j < j_limit; j++) {
				quat q_around_axis = euler_angles_deg_to_quat({0, 360 / 10 * j, 0});
				vec3 pos{r,0,0};
				pos = q_axis * q_around_axis * pos;
				vec3 initial_velocity_direction = generate_point_on_sphere(rand);
				vec3 initial_velocity = rand() * max_initial_speed * initial_velocity_direction;
				auto ball = make_unique<Sphere>(pos);
				ball->set_inverse_mass(1);
				ball->awake();
				ball->add_velocity(initial_velocity);
				set_name(ball.get(), "spawned ball" + std::to_string(++ball_counter));
				register_game_object(move(ball));
			}
		}
	}

	void Scene_base::spawn_boxes(int n) {
		// build random generatior with the same seed in order to have the same output at every run
		auto rand = bind(uniform_real_distribution<float>{0, 1}, default_random_engine{42});
		static int ball_counter = 0;
		constexpr float init_r = 5;
		constexpr float r_step = 2;
		constexpr float max_initial_speed = 10; // 100;
		float r = init_r;
		for (int i = 0; i < n / 10; i++, r += r_step) {
			int j_limit = i == n / 10 ? n % 10 : 10;
			vec3 axis = generate_point_on_sphere(rand);
			quat q_axis = glm::rotate(quat(), axis);
			for (int j = 0; j < j_limit; j++) {
				quat q_around_axis = euler_angles_deg_to_quat({0, 360 / 10 * j, 0});
				vec3 pos{r,0,0};
				pos = q_axis * q_around_axis * pos;
				vec3 initial_velocity_direction = generate_point_on_sphere(rand);
				vec3 initial_velocity = rand() * max_initial_speed * initial_velocity_direction;
				auto ball = make_unique<Box>(pos);
				ball->set_inverse_mass(1);
				ball->awake();
				ball->add_velocity(initial_velocity);
				set_name(ball.get(), "spawned box" + std::to_string(++ball_counter));
				register_game_object(move(ball));
			}
		}
	}

	void Scene_base::create_and_shoot_ball() {
		static int i = 0;

		vec3 pos = camera->transform->GetWorldPosition();
		quat orientation = camera->transform->GetWorldRotation();

		auto obj_h = make_unique<Sphere>(pos, 1, orientation);
		obj_h->set_inverse_mass(1);
		obj_h->set_asleep(false);
		set_name(obj_h.get(), "shooted sphere " + std::to_string(++i));
		float initial_speed = 12;
		obj_h->add_velocity(orientation * euler_angles_deg_to_quat(vec3{0,90,0}) * (initial_speed * vec3{1,0,0}));
		register_game_object(move(obj_h));
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

		{ // Create a shader program for drawing spheres with the color of the normal
			auto shader = make_unique<Shader>(Shader_id::sphere);
			shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.texture_sphere.VS.glsl", GL_VERTEX_SHADER);
			shader->AddShader(RESOURCE_PATH::SHADERS + "Normals.FS.glsl", GL_FRAGMENT_SHADER);
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
		int pairs_in_contact = 0;
		if (time_slowed) {
			delta_time_seconds /= 10;
		}
		if (!time_stopped || run_one_frame) {
			run_one_frame = false;

			// cap delta time
			float caped_delta_time = min(delta_time_seconds, 1.0f / 20);

			// apply forces
			force_registry.update_forces(caped_delta_time);

			// move bodies and update colliders in bvh (aka broad worst_collision phase)
			for (auto& rigid_body : rigid_bodies) {
				rigid_body->integrate(caped_delta_time);
			}

			// repair bvh
			bvh.clean_dirty_nodes();

			// narrow worst_collision phase
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

			modify_contacts(contacts);

			// resolve contacts
			if (!contacts.empty()) {
				// prepare resolver
				Contact_resolver contact_resolver(contacts, caped_delta_time); //DEMO1

				// solve contacts
				//contact_resolver.resolve_contacts(contacts); // DEMO1
				if (do_resolve_penetrations) {
					switch (penetration_type) {
					case 2:
						contact_resolver.resolve_penetrations_linearly(contacts);
						break;
					case 3:
						contact_resolver.resolve_penetrations_by_rotation(contacts);
						break;
					default:
						contact_resolver.resolve_penetrations(contacts);
					}
				}
				if (do_resolve_velocities) {
					contact_resolver.resolve_velocity(contacts);
				}
			}
		}

		if (destroy_objects_deep_down) {
			vector<gsl::not_null<Game_object*>> to_destroy;
			for (auto& game_object : game_objects) {
				if (game_object->get_transform().get_world_position().y < -100) {
					to_destroy.push_back(game_object.get());
				}
			}
			for (auto& obj : to_destroy) {
				unregister_game_object(obj);
			}
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
			ss << "obj_count:" << game_objects.size() << ";";
			frames_since_printing = 0;
			last_printing_time = current_time;
			ss << " broad contacts:" << bvh.get_contact_count() << ";";
			//ss << " pairs in narrow contact:" << pairs_in_contact << ";";
			//ss << " insertions:" << bvh.insertion_count << ";";
			//ss << " broad intersection checks:" << bvh.aabb_intersection_operations_count << ";";
			ss << " time:" << current_time << ";";
			//ss << " frames:" << total_frames << ";";
			continous_print_line_reset();
			continous_print(ss.str());
		}
	}

	void Scene_base::frame_end() {
		//draw_coordinat_system();
	}

	std::unique_ptr<Game_object> Scene_base::unregister_game_object(gsl::not_null<Game_object*> game_object) {
		return game_object->self_unregister(*this);
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

	void Scene_base::draw_coordinat_system2() {
		draw_coordinat_system2(camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}

	void Scene_base::draw_coordinat_system2(const glm::mat4& view_matrix, const glm::mat4& projection_maxtix) {
		Resource_manager& rm = get_resource_manager();
		// Render the coordinate system
		{
			const Shader& shader = rm.get_shader(Shader_id::color);
			shader.Use();
			glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_maxtix));



			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glLineWidth(3);
			object_model->SetScale(glm::vec3(1, 25, 1));
			object_model->SetWorldRotation(glm::quat());
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 0, 1, 0);
			simple_line->Render();


			object_model->SetWorldRotation(vec3{0,0,180});
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 0, 1, 0);
			simple_line->Render();

			object_model->SetWorldRotation(glm::vec3(0, 0, -90));
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 1, 0, 0);
			simple_line->Render();


			object_model->SetWorldRotation(vec3{0,0,90});
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 1, 0, 0);
			simple_line->Render();

			object_model->SetWorldRotation(glm::vec3(90, 0, 0));
			glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(object_model->GetModel()));
			glUniform3f(shader.GetUniformLocation("color"), 0, 0, 1);
			simple_line->Render();


			object_model->SetWorldRotation(vec3{-90,0,0});
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