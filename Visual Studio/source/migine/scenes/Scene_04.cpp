#include "Scene_04.h"

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
	Scene_04::Scene_04() {
	}

	Scene_04::~Scene_04() {
	}

	void Scene_04::init() {

		{ //defualt forces
			default_fs_gen.clear();
			//default_fs_gen.push_back(make_unique<Constant_torque_generator>(vec3{1,1,0}));
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{ 0, -10, 0 }, vec3{ 200, 20, 200 }, euler_angles_deg_to_quat({ 0,0,0 }))));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
			auto obj_h = make_unique<Sphere>(vec3{ -1, 2, -2 });
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "sfera");
		}


		{ // falling obj
			auto obj_h = make_unique<Box>(vec3{ 1, 2, -2 });
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "cub");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_04::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_04::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
		float caped_delta_time = min(deltaTimeSeconds, 1.0f / 20); // TODO nu e ok sa fie si aici si in Scene_base::update
	}

	void Scene_04::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_04::on_input_update(float deltaTime, int mods) {
	}

	void Scene_04::on_key_press(int key, int mods) {
		// add key press event
	}

	void Scene_04::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_04::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_04::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_04::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_04::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_04::on_window_resize(int width, int height) {
	}
}