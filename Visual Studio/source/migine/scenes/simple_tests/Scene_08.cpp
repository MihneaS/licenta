#include "Scene_08.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <Core/Engine.h>

#include <utility>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/game_objects/shapes/Debug_point.h>
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
using std::make_unique;
using std::vector;
using std::stringstream;

using std::cout;

namespace migine {
	Scene_08::Scene_08() {
	}

	Scene_08::~Scene_08() {
	}

	void Scene_08::init() {

		{ //defualt forces
			default_fs_gen.clear();
			default_fs_gen.push_back(make_unique<Gravity_generator>());
		}

		//{ // ground
		//	register_game_object(move(make_unique<Box>(vec3{ 0, -10, 0 }, vec3{ 200, 20, 200 })));
		//	set_name(game_objects.rbegin()->get(), "pamant");
		//}

		//{ // falling obj
		//	//auto obj_h = make_unique<Sphere>(vec3{0, 2, -2});
		//	auto obj_h = make_unique<Box>(vec3{ -1, 2, -2 });
		//	obj_h->set_inverse_mass(1);
		//	obj_h->set_motion(10 * k_sleep_epsilon);
		//	register_game_object(move(obj_h));
		//	set_name(game_objects.rbegin()->get(), "falling obj_1");
		//}

		//{ // rotated cube under sphere
		//	register_game_object(move(make_unique<Box>(vec3{ -1.5, 0.25, -2 }, vec3{ 1,1,1 }, euler_angles_deg_to_quat({ 45,0,45 }))));
		//	set_name(game_objects.rbegin()->get(), "static cube_1");
		//}

		{ // falling obj
			auto obj_h = make_unique<Box>(vec3{ 1, 2, -2.3f });
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "falling obj_2");
		}

		{ // rotated cube under sphere
			register_game_object(move(make_unique<Box>(vec3{ 1.6, 0.25, -2 }, vec3{ 1,1,1 }, euler_angles_deg_to_quat({ 45,0,45 }))));
			set_name(game_objects.rbegin()->get(), "static cube_2");
		}
		{ // make test point

			//register_game_object(move(make_unique<Debug_point>(vec3{0,1,0})));
			//set_name(game_objects.rbegin()->get(), "test_point_1");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_08::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_08::update(float deltaTimeSeconds) {
		// cap delta time
		float caped_delta_time = min(deltaTimeSeconds, 1.0f / 20);

		// apply forces
		force_registry.update_forces(caped_delta_time);

		// move bodies and update colliders in bvh (aka broad worst_collision phase)
		for (auto& rigid_body : rigid_bodies) {
			rigid_body->integrate(caped_delta_time);
		}

		// repair bvh
		bvh.clean_dirty_nodes();

		// narrow worst_collision phase
		int pairs_in_contact = 0;
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

			//vector<unique_ptr<Contact>> linearly;
			//vector<unique_ptr<Contact>> normaly;

			//for (auto& contact : contacts) {
			//	if (contact->objs[0]->id == cub1->id || contact->objs[1]->id == cub1->id ||
			//		contact->objs[0]->id == cub3->id || contact->objs[1]->id == cub3->id) {
			//		linearly.push_back(move(contact));
			//	}
			//	else {
			//		normaly.push_back(move(contact));
			//	}
			//}

			//// prepare resolver
			//Contact_resolver contact_resolver_l(linearly, caped_delta_time); //DEMO1
			//contact_resolver_l.resolve_penetrations_linearly_01(linearly);
			//Contact_resolver contact_resolver_n(normaly, caped_delta_time);
			//contact_resolver_n.resolve_penetrations_linearly(normaly);
			//contact_resolver_n.resolve_penetrations(normaly);

			// solve contacts

			Contact_resolver contact_resolver(contacts, caped_delta_time);
			//contact_resolver.resolve_contacts(contacts); // DEMO1
			contact_resolver.resolve_penetrations(contacts);

		}
		//for (auto& [obj0, obj1] : bvh.get_contacts()) {
		for (auto& contact : contacts) {
			auto& obj0 = contact->objs[0];
			auto& obj1 = contact->objs[1];
			obj0->set_inverse_mass(0);
			obj1->set_inverse_mass(0);
			obj0->stop_motion();
			obj1->stop_motion();
			force_registry.remove(obj0);
			force_registry.remove(obj1);
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
			// ss << "fps:" << frames_since_printing / delta_time_printing << ";";
			frames_since_printing = 0;
			last_printing_time = current_time;
			//ss << " broad contacts:" << bvh.get_contact_count() << ";";
			//ss << " pairs in narrow contact:" << pairs_in_contact << ";";
			// ss << " insertions:" << bvh.insertion_count << ";";
			//ss << " broad intersection checks:" << bvh.aabb_intersection_operations_count << ";";
			// ss << " time:" << current_time << ";";
			// ss << " frames:" << total_frames << ";";
			continous_print_line_reset();
			continous_print(ss.str());
		}
	}

	void Scene_08::frame_end() {
		Scene_base::frame_end();
	}

	void Scene_08::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}
} 