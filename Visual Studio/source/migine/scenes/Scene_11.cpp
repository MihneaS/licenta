#include "Scene_11.h"

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
	Scene_11::Scene_11() {
	}

	Scene_11::~Scene_11() {
	}

	void Scene_11::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		{ // ground
			register_game_object(move(make_unique<Box>(vec3{0, -10, 0}, vec3{200, 20, 200})));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{ // falling obj
			auto obj_h = make_unique<Box>(vec3{2, 2, -4});
			obj_h->set_inverse_mass(1);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj");
		}

		{ // obj on ground
			auto obj_h = make_unique<Box>(vec3{2, 0.5, -4});
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

		time_slowed = true;

	}

	void Scene_11::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_11::update(float deltaTimeSeconds) {
		int pairs_in_contact = 0;
		if (time_slowed) {
			deltaTimeSeconds /= 10;
		}
		if (!time_stopped) {
			// cap delta time
			float caped_delta_time = min(deltaTimeSeconds, 1.0f / 20);

			// apply forces
			force_registry.update_forces(caped_delta_time);

			// move bodies and update colliders in bvh (aka broad worst_collision phase)
			for (auto& rigid_body : rigid_bodies) {
				bool has_moved = rigid_body->integrate(caped_delta_time);
			}

			// repair bvh
			bvh.clean_dirty_nodes();

			// narrow worst_collision phase
			vector<unique_ptr<Contact>> contacts;
			for (auto& [obj0, obj1] : bvh.get_contacts()) {
				auto new_collisions = obj0->check_collision(*obj1);
				if (new_collisions.size() > 0) {
					pairs_in_contact++;
				}
				for (auto& new_collision : new_collisions) {
					contacts.push_back(move(new_collision));
				}
			}

			// resolve contacts
			if (!contacts.empty()) {
				// prepare resolver
				Contact_resolver contact_resolver(contacts, caped_delta_time); //DEMO1

				// solve contacts
				contact_resolver.resolve_contacts(contacts); // DEMO1
			}
		}

		// render
		for (auto& renderer : renderers) {
			renderer->render(this->get_scene_camera());
		}
		bvh.render_all(*camera);

		// print stats
		static float last_printing_time = 0;
		static int total_frames = 0;
		static int frames_since_printing = 0;
		frames_since_printing++;
		total_frames++;
		float current_time = get_elapsed_time();
		if (float delta_time_printing = current_time - last_printing_time; delta_time_printing > 0.66) {
			stringstream ss;
			ss << "fps:" << frames_since_printing / delta_time_printing << ";";
			frames_since_printing = 0;
			last_printing_time = current_time;
			ss << " broad contacts:" << bvh.get_contact_count() << ";";
			ss << " pairs in narrow contact:" << pairs_in_contact << ";";
			ss << " insertions:" << bvh.insertion_count << ";";
			//ss << " broad intersection checks:" << bvh.aabb_intersection_operations_count << ";";
			ss << " time:" << current_time << ";";
			ss << " frames:" << total_frames << ";";
			continous_print_line_reset();
			continous_print(ss.str());
		}
	}

	void Scene_11::frame_end() {
		draw_coordinat_system();
		Scene_base::frame_end();
	}

	void Scene_11::on_key_press(int key, int mods) {
		if (key == GLFW_KEY_T) {
			time_stopped = !time_stopped;
		}
		if (key == GLFW_KEY_Y) {
			time_slowed = !time_slowed;
		}
	}
}