#include "Scene_01.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <Core/Engine.h>

#include <utility>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/utils.h>
#include <migine/Resource_manager.h>
#include <migine/define.h>

using glm::vec3;
using glm::quat;

using std::move;
using std::make_unique;
using std::min;

using std::cout;

namespace migine {

	Box* tmp2;
	Sphere* tmp;


	Scene_01::Scene_01() :
			light_position(glm::vec3(0, 1, 1)),
			light_direction(glm::vec3(0, -1, 0)),
			material_shininess(30),
			material_kd(0.5),
			material_ks(0.5) {
	}

	Scene_01::~Scene_01() {
	}

	void Scene_01::init() {
		Resource_manager& rm = get_resource_manager();

		// create test Migine::Box
		{
			register_game_object(move(make_unique<Box>(vec3{2, 2, -2}, vec3{1, 2, 1}, euler_angles_deg_to_quat({45, 0, 45}))));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "cub rotit";
#endif // DEBUGGING
		}

		//create test Migine::Sphere
		{
			register_game_object(move(make_unique<Sphere>(vec3{-2, 2, -2})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera jos";
#endif // DEBUGGING
		}


		{
			register_game_object(move(make_unique<Sphere>(vec3{2, 5, 0})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera sus dreapta";
#endif // DEBUGGING
			register_game_object(move(make_unique<Sphere>(vec3{-2, 5, 0})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera sus stanga";
#endif // DEBUGGING
			register_game_object(move(make_unique<Sphere>(vec3{0, 5, 2})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera sus spate";
#endif // DEBUGGING
			register_game_object(move(make_unique<Sphere>(vec3{0, 5, -2})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera sus fata";
#endif // DEBUGGING
			register_game_object(move(make_unique<Sphere>(vec3{6, 5, 0})));
#ifdef DEBUGGING
			(*game_objects.rbegin())->name = "sfera sus foarte dreapta";
#endif // DEBUGGING
		}
		{
			auto s = make_unique<Sphere>(vec3{-6, 5, 0});
			tmp = s.get();
#ifdef DEBUGGING
			tmp->name = "sfera miscatoare";
#endif // DEBUGGING
			register_game_object(move(s));
		}
		{
			auto b = make_unique<Box>(vec3{0, 8, -2}, vec3{20, 0.2, 20});
#ifdef DEBUGGING
			b->name = "acoperitor";
#endif // DEBUGGING
			tmp2 = b.get();
			register_game_object(move(b));
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_01::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_01::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
		float caped_delta_time = min(deltaTimeSeconds, 1.0f / 20); // TODO nu e ok sa fie si aici si in Scene_base::update
		old_update(caped_delta_time);
	}

	void Scene_01::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	void Scene_01::render_simple_mesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model_matrix, glm::vec3 color) {
		if (!shader.GetProgramID()) {
			return;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// render an object using the specified shader and the specified position
		glUseProgram(shader.program);

		// Set shader uniforms for light & material properties
		// TODO: Set light position uniform
		int loc_light_position = glGetUniformLocation(shader.program, "light_position");
		glUniform3f(loc_light_position, light_position.x, light_position.y, light_position.z);

		int loc_light_direction = glGetUniformLocation(shader.program, "light_direction");
		glUniform3f(loc_light_direction, light_direction.x, light_direction.y, light_direction.z);

		// TODO: Set eye position (camera position) uniform
		glm::vec3 eye_position = get_scene_camera().transform->GetWorldPosition();
		int loc_eye_position = glGetUniformLocation(shader.program, "eye_position");
		glUniform3f(loc_eye_position, eye_position.x, eye_position.y, eye_position.z);

		// TODO: Set material property uniforms (shininess, kd, ks, object color) 
		int loc_material_shininess = glGetUniformLocation(shader.program, "material_shininess");
		glUniform1i(loc_material_shininess, material_shininess);

		int loc_material_kd = glGetUniformLocation(shader.program, "material_kd");
		glUniform1f(loc_material_kd, material_kd);

		int loc_material_ks = glGetUniformLocation(shader.program, "material_ks");
		glUniform1f(loc_material_ks, material_ks);

		int loc_object_color = glGetUniformLocation(shader.program, "object_color");
		glUniform3f(loc_object_color, color.r, color.g, color.b);

		// Bind model matrix
		GLint loc_model_matrix = glGetUniformLocation(shader.program, "Model");
		glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));

		// Bind view matrix
		glm::mat4 view_matrix = get_scene_camera().GetViewMatrix();
		int loc_view_matrix = glGetUniformLocation(shader.program, "View");
		glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

		// Bind projection matrix
		glm::mat4 projection_matrix = get_scene_camera().GetProjectionMatrix();
		int loc_projection_matrix = glGetUniformLocation(shader.program, "Projection");
		glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Draw the object
		glBindVertexArray(mesh.GetBuffers()->VAO);
		glDrawElements(mesh.GetDrawMode(), static_cast<int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_01::on_input_update(float deltaTime, int mods) {
		float speed = 2;

		if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 right = get_scene_camera().transform->GetLocalOXVector();
			glm::vec3 forward = get_scene_camera().transform->GetLocalOZVector();
			forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

			// Control light position using on W, A, S, D, E, Q
			if (window->KeyHold(GLFW_KEY_W)) light_position -= forward * deltaTime * speed;
			if (window->KeyHold(GLFW_KEY_A)) light_position -= right * deltaTime * speed;
			if (window->KeyHold(GLFW_KEY_S)) light_position += forward * deltaTime * speed;
			if (window->KeyHold(GLFW_KEY_D)) light_position += right * deltaTime * speed;
			if (window->KeyHold(GLFW_KEY_E)) light_position += up * deltaTime * speed;
			if (window->KeyHold(GLFW_KEY_Q)) light_position -= up * deltaTime * speed;
		}
	}

	void Scene_01::on_key_press(int key, int mods) {
		// add key press event
	}

	void Scene_01::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_01::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_01::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_01::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_01::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_01::on_window_resize(int width, int height) {
	}

	void Scene_01::old_update(float delta_time_seconds) {
		{
			glm::mat4 model_matrix = glm::mat4(1);
			model_matrix = glm::translate(model_matrix, glm::vec3(0, 1, 0));
			render_simple_mesh(get_mesh<Mesh_id::sphere>(), get_shader<Shader_id::lab8>(), model_matrix);
		}

		{
			glm::mat4 model_matrix = glm::mat4(1);
			model_matrix = glm::translate(model_matrix, glm::vec3(2, 0.5f, 0));
			model_matrix = glm::rotate(model_matrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
			model_matrix = glm::scale(model_matrix, glm::vec3(0.5f));
			render_simple_mesh(get_mesh<Mesh_id::box>(), get_shader<Shader_id::lab8>(), model_matrix);
		}

		{
			glm::mat4 model_matrix = glm::mat4(1);
			model_matrix = glm::translate(model_matrix, glm::vec3(-2, 0.5f, 0));
			model_matrix = glm::rotate(model_matrix, RADIANS(60.0f), glm::vec3(1, 1, 0));
			render_simple_mesh(get_mesh<Mesh_id::box>(), get_shader<Shader_id::lab8>(), model_matrix, glm::vec3(0, 0.5, 0));
		}

		// Render ground
		{
			glm::mat4 model_matrix = glm::mat4(1);
			model_matrix = glm::translate(model_matrix, glm::vec3(0, 0.01f, 0));
			model_matrix = glm::scale(model_matrix, glm::vec3(0.25f));
			render_simple_mesh(get_mesh<Mesh_id::plane>(), get_shader<Shader_id::lab8>(), model_matrix);
		}

		// Render the point light in the scene
		{
			glm::mat4 model_matrix = glm::mat4(1);
			model_matrix = glm::translate(model_matrix, light_position);
			model_matrix = glm::scale(model_matrix, glm::vec3(0.1f));
			render_mesh(get_mesh<Mesh_id::sphere>(), get_shader<Shader_id::lab8>(), model_matrix);
		}
	}
}