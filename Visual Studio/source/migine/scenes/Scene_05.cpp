#include "Scene_05.h"

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
using glm::mat4;
using glm::mat3;

using std::move;
using std::make_unique;
using std::min;

using std::cout;

namespace migine {

	void print_mat4(mat4 mat) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << mat[i][j] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	void print_mat3(mat3 mat) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << mat[i][j] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	Scene_05::Scene_05() {
	}

	Scene_05::~Scene_05() {
	}

	void Scene_05::init() {

		{ //defualt forces
			default_fs_gen.clear();
		}

		{
			migine::Transform transform({ 0.5,0.5,0.5 }, { 2,2,2 }, euler_angles_deg_to_quat({ 90,0,0 }));
			auto& to_world = transform.get_model();
			auto inv = glm::inverse(to_world);
			auto trsp_inv = glm::transpose(inv);
			mat3 some_mat3 = {{2,0,0}, {0,2,0}, {0,0,2}};
			mat4 some_mat4 = mat4(some_mat3);
			print_mat4(some_mat4);
			//mat4 coords_changed = inv * some_mat4 * trsp_inv;
			mat4 coords_changed = trsp_inv * some_mat4 * inv;
			//mat4 coords_changed2 = trsp_inv * some_mat3 * inv;
			print_mat4(coords_changed);
			mat3 m3 = coords_changed;
			print_mat3(m3);

			vec3 v1{ 1,0,0 };
			vec3 v2{ 0,1,0 };
			vec3 v3{ 0 };

			cout << "cross vs skew_mat:" << glm::cross(v1, v2).z << " " << (get_skew_symmetric(v1) * v2).z << "\n";


			vec3 y{0,1,0};
			vec3 x{1,0,0};
			quat qx(0, 1, 0, 0);
			quat qy(0, 0, 1, 0);
			quat q(cos(90 * k_deg_to_rad), vec3{1,0,0});
			cout << "y doar o rotatie: " << q * y << "\n";
			cout << "y ambele rotatii: " << q * y * glm::conjugate(q) << "\n";
			cout << "x doar o rotatie: " << q * x << "\n";
			cout << "x ambele rotatii: " << q * x * glm::conjugate(q) << "\n";
			cout << "qy doar o rotatie: " << q * qy << "\n";
			cout << "qy ambele rotatii: " << q * qy * glm::conjugate(q) << "\n";
			cout << "qx doar o rotatie: " << q * qx << "\n";
			cout << "qx ambele rotatii: " << q * qx * glm::conjugate(q) << "\n";
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_05::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_05::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_05::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_05::on_input_update(float deltaTime, int mods) {
	}

	void Scene_05::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}

	void Scene_05::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_05::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_05::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_05::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_05::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_05::on_window_resize(int width, int height) {
	}
}