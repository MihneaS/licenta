#include "Scene_17.h"

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
	Scene_17::Scene_17() {
	}

	Scene_17::~Scene_17() {
	}

	void Scene_17::init() {

		{ //defualt forces
			default_fs_gen.clear();
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -100, 0}, vec3{200, 200, 200})));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // obj 2-l
//auto obj_h = make_unique<Box>(vec3{-4, 2, -4}, vec3{1}, euler_angles_deg_to_quat(vec3{0,45,0}));
			auto obj_h = make_unique<Box>(vec3{-4, 2, -4}, vec3{1}, euler_angles_deg_to_quat(vec3{0,0,0}));
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			//obj_h->add_angular_velocity(vec3{0,1,0});
			obj_h->add_velocity(vec3{1,0,0});
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "obj 2-l");
		}
		{ // obj 2-r
			auto obj_h = make_unique<Box>(vec3{4, 2, -3.8}, vec3{1}, euler_angles_deg_to_quat(vec3{180+45,-45,45}));
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			//obj_h->add_angular_velocity(vec3{0,1.2,0});
			obj_h->add_velocity(vec3{-1,0,0});
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "obj 2-r");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_17::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_17::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_17::frame_end() {
		Scene_base::frame_end();
	}

	void Scene_17::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}
}