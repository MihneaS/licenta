#include "Contact_resolver.h"

#include <migine/game_objects/contact_detection/Collider_base.h>
#include <migine/make_array.h>

using glm::mat3;
using glm::vec3;
using glm::quat;
using glm::cross;
using glm::dot;

using gsl::not_null;

using std::array;
using std::vector;
using std::unique_ptr;

// worst_collision resolution, heavily inspired by Ian Millington's "Game Physics Engine" (as many other things in this project)
namespace migine {
	vec3 Contact_resolver::Additional_contact_data::calculate_local_velocity(not_null<const Contact*> contact, int obj_idx, float delta_time) { // delta_time will be used later
		vec3 velocity = contact->objs[obj_idx]->get_velocity();
		velocity += cross(contact->objs[obj_idx]->get_angular_velocity(), relative_contact_positions[obj_idx]);
		velocity = world_to_contact_rotation * velocity;
		return velocity;
	}

	Contact_resolver::Additional_contact_data::Additional_contact_data(not_null<const Contact*> contact, float delta_time) {
		world_to_contact_rotation = contact->get_rotation_to_contact_basis();
		contact_to_world_rotation = conjugate(world_to_contact_rotation);

		relative_contact_positions[0] = contact->contact_point - contact->objs[0]->transform.get_world_position();
		relative_contact_positions[1] = contact->contact_point - contact->objs[1]->transform.get_world_position();

		contact_local_velocity = calculate_local_velocity(contact, 0, delta_time) - calculate_local_velocity(contact, 1, delta_time);
	}

	Contact_resolver::Contact_resolver(const vector<unique_ptr<Contact>>& collisions, float delta_time) :
		delta_time(delta_time) {
		// precompute additional data
		for (const auto& collision : collisions) {
			additional_contact_data.emplace_back(collision.get(), delta_time);
		}
	}

	void Contact_resolver::resolve_contacts(vector<unique_ptr<Contact>>& contacts) {
		// resolve the interprenetrations problems with contacts
		resolve_penetrations(contacts);

		// resolve the velocity problems with contacts
		resolve_velocity(contacts);
	}

	void Contact_resolver::resolve_penetrations(vector<unique_ptr<Contact>>& contacts) {
		for (int position_iteration = 0; position_iteration < k_maximum_position_correcting_iterations; position_iteration++) {
			// find worst (deepest) worst_collision
			int worst_collision_idx = -1;
			float worst_penetration = k_penetration_epsilon;
			for (int i = 0; i < contacts.size(); i++) {
				if (contacts[i]->penetration_depth > worst_penetration) {
					worst_penetration = contacts[i]->penetration_depth;
					worst_collision_idx = i;
				}
			}
			if (worst_collision_idx < 0) {
				break;
			}

			// solve worst worst_collision
			// calculate linear and angular movement
			const Contact& worst_collision = *contacts[worst_collision_idx].get();
			const Additional_contact_data more_data = additional_contact_data[worst_collision_idx];
			array<float, 2> angular_inertia = {0};
			array<float, 2> linear_inertia = {0};
			float total_inertia = 0;
			for (int i = 0; i < 2; i++) {
				const mat3& inverse_inertia_tensor = worst_collision.objs[i]->get_inverse_invertia_tensor();
				// Use the same procedure as for calculating frictionless
				// velocity change to work out the angular inertia.
				vec3 angular_inertia_world = cross(more_data.relative_contact_positions[i], worst_collision.normal);
				angular_inertia_world = inverse_inertia_tensor * angular_inertia_world;
				angular_inertia_world = cross(angular_inertia_world, more_data.relative_contact_positions[i]);
				angular_inertia[i] = dot(angular_inertia_world, worst_collision.normal);
				// The linear component is simply the inverse mass.
				linear_inertia[i] = worst_collision.objs[i]->get_inverse_mass();
				// Keep track of the total inertia from all components.
				total_inertia += linear_inertia[i] + angular_inertia[i];
			}
			float inverse_inertia = 1 / total_inertia;
			auto linear_move = make_array(
				-worst_collision.penetration_depth * linear_inertia[0] * inverse_inertia,
				worst_collision.penetration_depth * linear_inertia[1] * inverse_inertia
			);
			auto angular_move = make_array(
				-worst_collision.penetration_depth * angular_inertia[0] * inverse_inertia,
				worst_collision.penetration_depth * angular_inertia[1] * inverse_inertia
			);

			// avoid excesive rotation
			for (int i = 0; i < 2; i++) {
				float limit = k_angular_move_limit * more_data.relative_contact_positions[i].length();
				// Check the angular move is within limits.
				if (fabs(angular_move[i]) > limit) {
					float total_move = linear_move[i] + angular_move[i];
					// Set the new angular move, with the same sign as before.
					angular_move[i] = copysign(angular_move[i], limit);
					// Make the linear move take the extra slack.
					linear_move[i] = total_move - angular_move[i];
				}
			}

			// apply move
			vec3 position_change[2], orientation_change[2];
			for (int i = 0; i < 2; i++) {
				// apply linear move
				position_change[i] = worst_collision.normal * linear_move[i];
				worst_collision.objs[i]->transform.change_position_with_delta(position_change[i]);

				// apply angular move
				if (!is_zero_aprox(angular_inertia[i])) {
					const mat3& inverse_inertia_tensor = worst_collision.objs[i]->get_inverse_invertia_tensor();
					vec3 impulsive_torque = cross(more_data.relative_contact_positions[i], worst_collision.normal);
					vec3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
					vec3 rotation_per_move = impulse_per_move / angular_inertia[i];
					orientation_change[i] = rotation_per_move * angular_move[i];
					worst_collision.objs[i]->transform.change_orientation_with_delta(orientation_change[i]);
				}
			}

			// update penetrations and velocities
			for (int i = 0; i < contacts.size(); i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						if (contacts[i]->objs[j] == worst_collision.objs[k]) {
							// update penetrations
							vec3 cross_product = cross(orientation_change[k], additional_contact_data[i].relative_contact_positions[j]);
							cross_product += position_change[k];
							//int sign = -1 + 2 * j; // = j ? 1 : -1;
							int sign = 1 - 2 * j; // = j ? -1 : 1;
							contacts[i]->penetration_depth += sign * dot(cross_product, contacts[i]->normal);
						}
					}
				}
			}
			assert(worst_penetration > worst_collision.penetration_depth); // assert that penetration depth was improved
		}
	}

	void Contact_resolver::resolve_velocity(std::vector<std::unique_ptr<Contact>>& contacts) {
		for (int velocity_iteration = 0; velocity_iteration < k_maximum_velocity_correcting_iterations; velocity_iteration++) {
			// find worst (deepest) worst_collision
			int worst_collision_idx = -1;
			float worst_velocity2 = k_velocity_epsilon;
			for (int i = 0; i < contacts.size(); i++) {
				if (float vel_mag2 = length2(additional_contact_data[i].contact_local_velocity); vel_mag2 > worst_velocity2) {
					worst_velocity2 = vel_mag2;
					worst_collision_idx = i;
				}
			}
			if (worst_collision_idx < 0) {
				break;
			}

			// solve worst worst_collision
			// calculate linear and angular movement
			const Contact& worst_collision = *contacts[worst_collision_idx].get();
			const Additional_contact_data more_data = additional_contact_data[worst_collision_idx];
			array<float, 2> angular_inertia = {0};
			array<float, 2> linear_inertia = {0};
			float total_inertia = 0;
			for (int i = 0; i < 2; i++) {
				const mat3& inverse_inertia_tensor = worst_collision.objs[i]->get_inverse_invertia_tensor();
				// Use the same procedure as for calculating frictionless
				// velocity change to work out the angular inertia.
				vec3 angular_inertia_world = cross(more_data.relative_contact_positions[i], worst_collision.normal);
				angular_inertia_world = inverse_inertia_tensor * angular_inertia_world;
				angular_inertia_world = cross(angular_inertia_world, more_data.relative_contact_positions[i]);
				angular_inertia[i] = dot(angular_inertia_world, worst_collision.normal);
				// The linear component is simply the inverse mass.
				linear_inertia[i] = worst_collision.objs[i]->get_inverse_mass();
				// Keep track of the total inertia from all components.
				total_inertia += linear_inertia[i] + angular_inertia[i];
			}
			float inverse_inertia = 1 / total_inertia;
			auto linear_move = make_array(
				-worst_collision.penetration_depth * linear_inertia[0] * inverse_inertia,
				worst_collision.penetration_depth * linear_inertia[1] * inverse_inertia
			);
			auto angular_move = make_array(
				-worst_collision.penetration_depth * angular_inertia[0] * inverse_inertia,
				worst_collision.penetration_depth * angular_inertia[1] * inverse_inertia
			);

			// avoid excesive rotation
			for (int i = 0; i < 2; i++) {
				float limit = k_angular_move_limit * more_data.relative_contact_positions[i].length();
				// Check the angular move is within limits.
				if (fabs(angular_move[i]) > limit) {
					float total_move = linear_move[i] + angular_move[i];
					// Set the new angular move, with the same sign as before.
					angular_move[i] = copysign(angular_move[i], limit);
					// Make the linear move take the extra slack.
					linear_move[i] = total_move - angular_move[i];
				}
			}

			// apply move
			vec3 position_change[2], orientation_change[2];
			for (int i = 0; i < 2; i++) {
				// apply linear move
				position_change[i] = worst_collision.normal * linear_move[i];
				worst_collision.objs[i]->transform.change_position_with_delta(position_change[i]);

				// apply angular move
				if (!is_zero_aprox(angular_inertia[i])) {
					const mat3& inverse_inertia_tensor = worst_collision.objs[i]->get_inverse_invertia_tensor();
					vec3 impulsive_torque = cross(more_data.relative_contact_positions[i], worst_collision.normal);
					vec3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
					vec3 rotation_per_move = impulse_per_move / angular_inertia[i];
					orientation_change[i] = rotation_per_move * angular_move[i];
					worst_collision.objs[i]->transform.change_orientation_with_delta(orientation_change[i]);
				}
			}

			// update penetrations and velocities
			for (int i = 0; i < contacts.size(); i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						if (contacts[i]->objs[j] == worst_collision.objs[k]) {
							// update penetrations
							vec3 cross_product = cross(orientation_change[k], additional_contact_data[i].relative_contact_positions[j]);
							cross_product += position_change[k];
							//int sign = -1 + 2 * j; // = j ? 1 : -1;
							int sign = 1 - 2 * j; // = j ? -1 : 1;
							contacts[i]->penetration_depth += sign * dot(cross_product, contacts[i]->normal);
						}
					}
				}
			}
			assert(worst_velocity2 > worst_collision.penetration_depth); // assert that penetration depth was improved
		}
	}


	void Contact_resolver::calculate_desired_delta_velocity(Contact& contact, Additional_contact_data& more_data, float delta_time) {

		// Calculate the acceleration induced velocity accumulated this frame
		float velocity_from_acc = 0;

		velocity_from_acc += dot(contact.objs[0]->get_last_frame_acceleration() * delta_time, contact.normal);
		velocity_from_acc -= dot(contact.objs[1]->get_last_frame_acceleration() * delta_time, contact.normal);

		// If the velocity is very slow, limit the restitution
		float this_restitution = k_default_restitution;
		if (abs(more_data.contact_local_velocity.y) < k_velocity_restitution_limit) {
			this_restitution = 0.0f;
		}

		// Combine the bounce velocity with the removed
		// acceleration velocity.
		more_data.desired_delta_velocity = -more_data.contact_local_velocity.y - this_restitution * (more_data.contact_local_velocity.y - velocity_from_acc);
	}
}
