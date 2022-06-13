#include "Scene_12.h"

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
using std::min;
using std::unique_ptr;
using std::swap;
using std::vector;
using std::stringstream;

using std::cout;

namespace migine {
	Scene_12::Scene_12() {
	}

	Scene_12::~Scene_12() {
	}

	void Scene_12::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -100, 0}, vec3{200, 200, 200})));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
			auto obj_h = make_unique<Box>(vec3{2, 2, -4}, vec3{1}, euler_angles_deg_to_quat(vec3{0,0,15}));
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_12::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_12::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_12::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	void Scene_12::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}
	void Scene_12::modify_contacts(std::vector<std::unique_ptr<Contact>>& contacts) {
		for (auto& contact : contacts) {
			if (contact->objs[1]->name == "falling obj 1" || contact->objs[0]->name == "falling obj 2") {
				swap(contact->objs[0], contact->objs[1]);
				contact->normal *= -1;
			}
		}
	}
}