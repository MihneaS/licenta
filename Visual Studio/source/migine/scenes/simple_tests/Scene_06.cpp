#include "Scene_06.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <Core/Engine.h>

#include <utility>
#include <sstream>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/utils.h>
#include <migine/Resource_manager.h>
#include <migine/define.h>
#include <migine/physics/Contact_resolver.h>

#include <gsl/gsl>

using glm::vec3;
using glm::quat;

using std::vector;
using std::move;
using std::unique_ptr;
using std::make_unique;
using std::min;
using std::stringstream;

using std::cout;

using gsl::not_null;

namespace migine {
	Scene_06::Scene_06() {
	}

	Scene_06::~Scene_06() {
	}

	void Scene_06::init() {

		{ // set camera position and rotation
			camera->transform->SetWorldPosition(vec3(0, 2.8f, 4));
			camera->transform->SetWorldRotation(vec3(-15, 0, 0));
			camera->Update();
		}

		{ //defualt forces
			default_fs_gen.clear();
			//default_fs_gen.push_back(make_unique<Sinusoidal_torque_generator>(vec3{ 1,1,0 }));
			//default_fs_gen.push_back(make_unique<Gravity_generator>());
			default_fs_gen.push_back(make_unique<Constant_force_generator>(vec3{0,-20,0}));
		}

		{ // ground
			auto obj_h = make_unique<Box>(vec3{ 0, -10, 0 }, vec3{ 200, 20, 200 }, euler_angles_deg_to_quat({ 0,0,0 }));
			pamant = obj_h.get();
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "pamant");
		}

		{
			auto obj_h = make_unique<Box>(vec3{ -4, 20, -2 });
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			cub1 = obj_h.get();
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "cub_1");
		} 
		
		{
			auto obj_h = make_unique<Box>(vec3{ -1, 20, -2 });
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			cub2 = obj_h.get();
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "cub_2");
		}

		{
			auto obj_h = make_unique<Box>(vec3{ 1, 20, -2 }, vec3{1}, euler_angles_deg_to_quat(vec3{0, 45, 45}));
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			cub3 = obj_h.get();
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "cub_3");
		}

		{
			auto obj_h = make_unique<Box>(vec3{ 4, 20, -2 }, vec3{ 1 }, euler_angles_deg_to_quat(vec3{ 0, 45, 45 }));
			obj_h->set_inverse_mass(1);
			obj_h->set_motion(10 * k_sleep_epsilon);
			cub4 = obj_h.get();
			register_game_object(move(obj_h));
			set_name(game_objects.rbegin()->get(), "cub_4");
		}

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_06::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_06::update(float deltaTimeSeconds) {
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
		assert(pamant->get_inverse_mass() == 0);

		// resolve contacts
		if (!contacts.empty()) {

			vector<unique_ptr<Contact>> linearly;
			vector<unique_ptr<Contact>> normaly;

			for (auto& contact : contacts) {
				if (contact->objs[0]->id == cub1->id || contact->objs[1]->id == cub1->id ||
					contact->objs[0]->id == cub3->id || contact->objs[1]->id == cub3->id) {
					linearly.push_back(move(contact));
				} else {
					normaly.push_back(move(contact));
				}
			}

			// prepare resolver
			Contact_resolver contact_resolver_l(linearly, caped_delta_time); //DEMO1
			contact_resolver_l.resolve_penetrations_linearly(linearly);
			Contact_resolver contact_resolver_n(normaly, caped_delta_time);
			//contact_resolver_n.resolve_penetrations_linearly(normaly);
			contact_resolver_n.resolve_penetrations(normaly);

			// solve contacts
			//contact_resolver.resolve_contacts(contacts); // DEMO1
			//contact_resolver.resolve_penetrations(contacts);

		}
		for (auto& [obj0, obj1] : bvh.get_contacts()) {
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
			// ss << "1" << cub1->transform.get_orientation() << ";";
			// ss << "2" << cub2->transform.get_orientation() << ";";
			ss << "3" << cub3->transform.get_orientation() << ";";
			ss << "4" << cub4->transform.get_orientation() << ";";
			continous_print_line_reset();
			continous_print(ss.str());
		}
	}

	void Scene_06::frame_end() {
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_06::on_input_update(float deltaTime, int mods) {
	}

	void Scene_06::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}

	void Scene_06::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_06::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_06::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_06::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_06::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_06::on_window_resize(int width, int height) {
	}
}
