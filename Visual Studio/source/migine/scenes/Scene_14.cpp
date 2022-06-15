#include "Scene_14.h"

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
	Scene_14::Scene_14() {
	}

	Scene_14::~Scene_14() {
	}

	void Scene_14::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -10, 0}, vec3{200, 20, 200})));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
			//auto obj_h = make_unique<Box>(vec3{2, 1.5, -4});
			auto obj_h = make_unique<Box>(vec3{2, 1.51, -4});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj");
		}

		{ // obj on ground
			auto obj_h = make_unique<Box>(vec3{2, 0.5, -4}, vec3{1.2, 1, 1.2});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "obj on ground");
		}

		{ // rotated cube under sphere
			//register_game_object(move(make_unique<Box>(vec3{1.5, 0.25, -4}, vec3{1,1,1}, euler_angles_deg_to_quat({-45,0,-45}))));
			//set_name(game_objects.rbegin()->get(), "static cube");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}
	}

	void Scene_14::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_14::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_14::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	void Scene_14::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
		if (key == GLFW_KEY_SPACE) {
			create_and_shoot_ball();
		}
	}

	void Scene_14::create_and_shoot_ball() {
		static int i = 0;

		vec3 pos = camera->transform->GetWorldPosition();
		quat orientation = camera->transform->GetWorldRotation();

		auto obj_h = make_unique<Sphere>(pos, 1, orientation);
		obj_h->set_inverse_mass(1);
		obj_h->set_asleep(false);
		set_name(obj_h.get(), "shooted sphere " + std::to_string(++i));
		float initial_speed = 25;
		obj_h->add_velocity(orientation * euler_angles_deg_to_quat(vec3{0,90,0}) * (initial_speed * vec3{1,0,0}));
		register_game_object(move(obj_h));
	}
}