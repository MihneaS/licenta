#include "Scene_21.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <Core/Engine.h>

#include <utility>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Wall.h>
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
	Scene_21::Scene_21() {
	}

	Scene_21::~Scene_21() {
	}

	void Scene_21::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<To_center_gravity_generator>());
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}
	}

	void Scene_21::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_21::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_21::frame_end() {
		draw_coordinat_system2();
		Scene_base::frame_end();
	}

	void Scene_21::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}
}