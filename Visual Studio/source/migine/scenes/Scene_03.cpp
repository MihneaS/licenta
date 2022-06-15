#include "Scene_03.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <Core/Engine.h>

#include <utility>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/utils.h>
#include <migine/Resource_manager.h>
#include <migine/define.h>
#include <migine/physics/Contact_resolver.h>

using glm::vec3;
using glm::quat;

using std::move;
using std::make_unique;
using std::unique_ptr;
using std::min;
using std::vector;
using std::stringstream;
using std::swap;

using std::cout;

namespace migine {
	Scene_03::Scene_03() {
	}

	Scene_03::~Scene_03() {
	}

	void Scene_03::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -10, 0}, vec3{200, 20, 200}, euler_angles_deg_to_quat({0,0,25}))));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
			auto obj_h = make_unique<Sphere>(vec3{0, 2, -2});
			//auto obj_h = make_unique<Box>(vec3{0, 2, -2});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj 1");
		}

		if (true)
		{ // falling obj
			auto obj_h = make_unique<Sphere>(vec3{ 0, 2, -4 });
			//auto obj_h = make_unique<Box>(vec3{0, 2, -2});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj 2");
		}

		{ // rotated cube under sphere
			//register_game_object(move(make_unique<Sphere>(vec3{-0.5, 0.25, -2}, 1, euler_angles_deg_to_quat({45,0,45}))));
			//set_name(game_objects.rbegin()->get(), "static sphere");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_03::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_03::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_03::frame_end() {
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_03::on_input_update(float deltaTime, int mods) {
	}

	void Scene_03::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}

	void Scene_03::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_03::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_03::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_03::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_03::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_03::on_window_resize(int width, int height) {
	}

	void Scene_03::modify_contacts(std::vector<std::unique_ptr<Contact>>& contacts) {
		for (auto& contact : contacts) {
			if (contact->objs[1]->name == "falling obj 1") {
				swap(contact->objs[0], contact->objs[1]);
				contact->normal *= -1;
			}
		}
	}
}