#include "Scene_20.h"

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
	Scene_20::Scene_20() {
	}

	Scene_20::~Scene_20() {
	}

	void Scene_20::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // walls
			spawn_walls_small();
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}
	}

	void Scene_20::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_20::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_20::frame_end() {
		Scene_base::frame_end();
	}

	void Scene_20::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}
}