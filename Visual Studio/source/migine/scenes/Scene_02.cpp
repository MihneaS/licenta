#include "Scene_02.h"

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
	Scene_02::Scene_02() {
	}

	Scene_02::~Scene_02() {
	}

	void Scene_02::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -10, 0}, vec3{200, 20, 200})));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
		    //auto obj_h = make_unique<Sphere>(vec3{0, 2, -2});
			auto obj_h = make_unique<Box>(vec3{0, 2, -2});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj");
		}

		{ // rotated cube under sphere
			register_game_object(move(make_unique<Box>(vec3{-0.5, 0.25, -2}, vec3{1,1,1}, euler_angles_deg_to_quat({45,0,45}))));
			set_name(game_objects.rbegin()->get(), "static cube");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_02::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_02::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
		float caped_delta_time = min(deltaTimeSeconds, 1.0f / 20); // TODO nu e ok sa fie si aici si in Scene_base::update
	}

	void Scene_02::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_02::on_input_update(float deltaTime, int mods) {
	}

	void Scene_02::on_key_press(int key, int mods) {
		// add key press event
	}

	void Scene_02::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_02::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_02::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_02::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_02::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_02::on_window_resize(int width, int height) {
	}
}