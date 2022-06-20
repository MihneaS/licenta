#include "Contact_resolver.h"

#include <migine/contact_detection/Collider_base.h>
#include <migine/make_array.h>
#include <migine/constants.h>

#include <migine/scenes/current_scene.h>
#include <migine/game_objects/shapes/Box.h>
#include <migine/utils.h>

#include <migine/define.h>

using glm::mat3;
using glm::vec3;
using glm::quat;
using glm::cross;
using glm::dot;
using glm::conjugate;
using glm::transpose;

using gsl::not_null;

using std::array;
using std::vector;
using std::unique_ptr;
using std::tuple;
using std::isfinite;

// worst_contact resolution, heavily inspired by Ian Millington's "Game Physics Engine" (as many other things in this project)
namespace migine {
	Contact_resolver::Additional_contact_data::Additional_contact_data(not_null<const Contact*> contact, float delta_time) {
		world_to_contact_rotation = contact->get_rotation_to_contact_basis();
		contact_to_world_rotation = conjugate(world_to_contact_rotation);

		relative_contact_positions[0] = contact->contact_point - contact->objs[0]->transform.get_world_position();
		relative_contact_positions[1] = contact->contact_point - contact->objs[1]->transform.get_world_position();

		contact_local_velocity = calculate_local_velocity(contact, 1, delta_time) - calculate_local_velocity(contact, 0, delta_time); // TODO o fi corect? era cu minus
		assert(is_finite(contact_local_velocity));
		calculate_desired_delta_velocity(contact, delta_time);
	}

	vec3 Contact_resolver::Additional_contact_data::calculate_local_velocity(not_null<const Contact*> contact, int obj_idx, float delta_time) { // delta_time will be used later
		vec3 velocity = contact->objs[obj_idx]->get_velocity();
		velocity += cross(contact->objs[obj_idx]->get_angular_velocity(), relative_contact_positions[obj_idx]);
		velocity = world_to_contact_rotation * velocity;
		assert(is_finite(velocity));
		return velocity;
	}

	void Contact_resolver::Additional_contact_data::calculate_desired_delta_velocity(gsl::not_null<const Contact*> contact, float delta_time) {

		// Calculate the acceleration induced velocity accumulated this frame
		float velocity_from_acc = 0;

		// TODO verifica
		velocity_from_acc -= dot(contact->objs[0]->get_last_frame_acceleration() * delta_time, contact->normal);
		velocity_from_acc += dot(contact->objs[1]->get_last_frame_acceleration() * delta_time, contact->normal); // aici era plus

		// If the velocity is very slow, limit the restitution
		float this_restitution = k_default_restitution;
		if (abs(contact_local_velocity.y) < k_velocity_restitution_limit) {
			this_restitution = 0.0f;
		}

		// Combine the bounce velocity with the removed
		// acceleration velocity.
 		//desired_delta_velocity = -contact_local_velocity.y - this_restitution * (contact_local_velocity.y - velocity_from_acc);
		desired_delta_velocity = -contact_local_velocity.y * (1 + this_restitution) - velocity_from_acc;
		assert(isfinite(desired_delta_velocity));
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
		//resolve_penetrations_linearly_01(contacts);

		// resolve the velocity problems with contacts
		resolve_velocity(contacts);
	}

	void Contact_resolver::resolve_penetrations(vector<unique_ptr<Contact>>& contacts) {
		for (int position_iteration = 0; position_iteration < k_maximum_position_correcting_iterations; position_iteration++) {
			// find worst (deepest) worst_contact
			int worst_collision_idx = -1;
			float worst_penetration = k_penetration_epsilon;
			for (int i = 0; i < contacts.size(); i++) {
				//if (contacts[i]->penetration_depth > worst_penetration && !contacts[i]->pen_resolved) {
				if (contacts[i]->penetration_depth > worst_penetration) {
					worst_penetration = contacts[i]->penetration_depth;
					worst_collision_idx = i;
				}
			}
			if (worst_collision_idx < 0) {
				break;
			}

			// solve worst worst_contact
			// calculate linear and angular movement
			//contacts[worst_collision_idx]->pen_resolved = true;
			const Contact& worst_contact = *contacts[worst_collision_idx].get();
			const Additional_contact_data more_data = additional_contact_data[worst_collision_idx];

			if (!worst_contact.objs[0]->is_asleep() || !worst_contact.objs[1]->is_asleep()) {
				worst_contact.objs[0]->set_asleep(false);
				worst_contact.objs[1]->set_asleep(false);
			}

			//if (worst_contact.objs[0]->get_inverse_mass() != 0 && worst_contact.objs[1]->get_inverse_mass() != 0) {
			//	int i = 0;
			//	assert(true);
			//	i++;
			//}

			array<float, 2> angular_inertia = {0};
			array<float, 2> linear_inertia = {0};
			float total_inertia = 0;
			for (int i = 0; i < 2; i++) {
				const mat3& inverse_inertia_tensor = worst_contact.objs[i]->get_inverse_invertia_tensor_world();
				// Use the same procedure as for calculating frictionless
				// velocity change to work out the angular inertia.
				vec3 angular_inertia_world = cross(more_data.relative_contact_positions[i], worst_contact.normal);
				angular_inertia_world = inverse_inertia_tensor * angular_inertia_world;
				angular_inertia_world = cross(angular_inertia_world, more_data.relative_contact_positions[i]);
				angular_inertia[i] = dot(angular_inertia_world, worst_contact.normal);
				// The linear component is simply the inverse mass.
				linear_inertia[i] = worst_contact.objs[i]->get_inverse_mass();
				// Keep track of the total inertia from all components.
				total_inertia += linear_inertia[i] + angular_inertia[i];
			}
			float inverse_inertia = 1 / total_inertia;
			auto linear_move = make_array(
				-worst_contact.penetration_depth * linear_inertia[0] * inverse_inertia,
				worst_contact.penetration_depth * linear_inertia[1] * inverse_inertia
			);
			auto angular_move = make_array(
				-worst_contact.penetration_depth * angular_inertia[0] * inverse_inertia,
				worst_contact.penetration_depth * angular_inertia[1] * inverse_inertia
			);

			// avoid excesive rotation
			for (int i = 0; i < 2; i++) {
				float limit = k_angular_move_limit * length(more_data.relative_contact_positions[i]);
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
				position_change[i] = worst_contact.normal * linear_move[i];
				worst_contact.objs[i]->transform.change_position_with_delta(position_change[i]);

				// apply angular move
				if (!is_zero_aprox(angular_inertia[i])) {
					const mat3& inverse_inertia_tensor = worst_contact.objs[i]->get_inverse_invertia_tensor_world();
					vec3 impulsive_torque = cross(more_data.relative_contact_positions[i], worst_contact.normal);
					vec3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
					vec3 rotation_per_move = impulse_per_move / angular_inertia[i];
					orientation_change[i] = rotation_per_move * angular_move[i];
					worst_contact.objs[i]->transform.change_orientation_with_delta(orientation_change[i]);
				}
			}

			// update penetrations
			for (int i = 0; i < contacts.size(); i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						if (contacts[i]->objs[j] == worst_contact.objs[k]) {
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
			assert(worst_penetration >= worst_contact.penetration_depth); // assert that penetration depth was improved
		}
	}

	void Contact_resolver::resolve_penetrations_linearly(vector<unique_ptr<Contact>>& contacts) {
		for (int position_iteration = 0; position_iteration < k_maximum_position_correcting_iterations; position_iteration++) {
			// find worst (deepest) worst_contact
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

			// solve worst worst_contact
			// calculate linear and angular movement
			const Contact& worst_contact = *contacts[worst_collision_idx].get();
			const Additional_contact_data more_data = additional_contact_data[worst_collision_idx];
			array<float, 2> linear_inertia = { 0 };
			float total_inertia = 0;
			for (int i = 0; i < 2; i++) {
				linear_inertia[i] = worst_contact.objs[i]->get_inverse_mass();
				// Keep track of the total inertia from all components.
				total_inertia += linear_inertia[i];
			}
			float inverse_inertia = 1 / total_inertia;
			auto linear_move = make_array(
				-worst_contact.penetration_depth * linear_inertia[0] * inverse_inertia,
				worst_contact.penetration_depth * linear_inertia[1] * inverse_inertia
			);

			// apply move
			vec3 position_change[2], orientation_change[2];
			for (int i = 0; i < 2; i++) {
				// apply linear move
				position_change[i] = worst_contact.normal * linear_move[i];
				worst_contact.objs[i]->transform.change_position_with_delta(position_change[i]);
			}

			// update penetrations and velocities
			for (int i = 0; i < contacts.size(); i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						if (contacts[i]->objs[j] == worst_contact.objs[k]) {
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
			assert(worst_penetration >= worst_contact.penetration_depth); // assert that penetration depth was improved
		}
	}

	void Contact_resolver::resolve_penetrations_by_rotation(vector<unique_ptr<Contact>>& contacts) {
		for (int position_iteration = 0; position_iteration < k_maximum_position_correcting_iterations; position_iteration++) {
			// find worst (deepest) worst_contact
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

			// solve worst worst_contact
			// calculate linear and angular movement
			Contact& worst_contact = *contacts[worst_collision_idx].get();
			const Additional_contact_data more_data = additional_contact_data[worst_collision_idx];

			//if (worst_contact.objs[0]->get_inverse_mass() != 0 && worst_contact.objs[1]->get_inverse_mass() != 0) {
			//	int i = 0;
			//	assert(true);
			//	i++;
			//}

			array<float, 2> angular_inertia = {0};
			float total_inertia = 0;
			for (int i = 0; i < 2; i++) {
				const mat3& inverse_inertia_tensor = worst_contact.objs[i]->get_inverse_invertia_tensor_world();
				// Use the same procedure as for calculating frictionless
				// velocity change to work out the angular inertia.
				vec3 angular_inertia_world = cross(more_data.relative_contact_positions[i], worst_contact.normal);
				angular_inertia_world = inverse_inertia_tensor * angular_inertia_world;
				angular_inertia_world = cross(angular_inertia_world, more_data.relative_contact_positions[i]);
				angular_inertia[i] = dot(angular_inertia_world, worst_contact.normal);
				// Keep track of the total inertia from all components.
				total_inertia += angular_inertia[i];
			}
			float inverse_inertia = 1 / total_inertia;
			auto angular_move = make_array(
				-worst_contact.penetration_depth * angular_inertia[0] * inverse_inertia,
				worst_contact.penetration_depth * angular_inertia[1] * inverse_inertia
			);

			// avoid excesive rotation
			for (int i = 0; i < 2; i++) {
				float limit = k_angular_move_limit * length(more_data.relative_contact_positions[i]);
				// Check the angular move is within limits.
				if (fabs(angular_move[i]) > limit) {
					float total_move = angular_move[i];
					// Set the new angular move, with the same sign as before.
					angular_move[i] = copysign(angular_move[i], limit);
				}
			}

			// apply move
			vec3 orientation_change[2];
			for (int i = 0; i < 2; i++) {

				// apply angular move
				if (!is_zero_aprox(angular_inertia[i])) {
					const mat3& inverse_inertia_tensor = worst_contact.objs[i]->get_inverse_invertia_tensor_world();
					vec3 impulsive_torque = cross(more_data.relative_contact_positions[i], worst_contact.normal);
					vec3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
					vec3 rotation_per_move = impulse_per_move / angular_inertia[i];
					orientation_change[i] = rotation_per_move * angular_move[i];
					worst_contact.objs[i]->transform.change_orientation_with_delta(orientation_change[i]);
				}
			}

			// update penetrations
			worst_contact.penetration_depth = 0;
		}
	}

	void Contact_resolver::resolve_velocity(std::vector<std::unique_ptr<Contact>>& contacts) {
		for (int velocity_iteration = 0; velocity_iteration < k_maximum_velocity_correcting_iterations; velocity_iteration++) {
			// find worst (deepest) worst_contact
			int worst_collision_idx = -1;
			float worst_velocity = k_velocity_epsilon;
			for (int i = 0; i < contacts.size(); i++) {
				//if (additional_contact_data[i].desired_delta_velocity > worst_velocity && !contacts[i]->vel_resolved) {
				if (additional_contact_data[i].desired_delta_velocity > worst_velocity) {
					worst_velocity = additional_contact_data[i].desired_delta_velocity;
					worst_collision_idx = i;
				}
			}
			if (worst_collision_idx < 0) {
				break;
			}

			// solve worst worst_contact
			// calculate linear and angular movement
			//contacts[worst_collision_idx]->vel_resolved = true;
			const Contact& worst_contact = *contacts[worst_collision_idx].get();
			const Additional_contact_data& more_data = additional_contact_data[worst_collision_idx];
			float debug_original_desired_delta_velocity = more_data.desired_delta_velocity;

			if (!worst_contact.objs[0]->is_asleep() || !worst_contact.objs[1]->is_asleep()) {
				worst_contact.objs[0]->set_asleep(false);
				worst_contact.objs[1]->set_asleep(false);
			}

			// apply move
			auto [velocity_change, rotation_change] = apply_move(*contacts[worst_collision_idx].get(), additional_contact_data[worst_collision_idx]);
			
			// update velocities
			for (int i = 0; i < contacts.size(); i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						if (contacts[i]->objs[j] == worst_contact.objs[k]) {
							vec3 delta_vel = velocity_change[k] + cross(rotation_change[k], more_data.relative_contact_positions[j]);

							float sign = -1 + 2 * j; // = j ? -1 : 1;
							auto tmp = conjugate(additional_contact_data[i].contact_to_world_rotation) * delta_vel * sign;
							additional_contact_data[i].contact_local_velocity += tmp;
							assert(is_finite(additional_contact_data[i].contact_local_velocity));
							additional_contact_data[i].calculate_desired_delta_velocity(&worst_contact, delta_time);
						}
					}
				}
			}
			
			assert(more_data.desired_delta_velocity < debug_original_desired_delta_velocity); // assert that desired velocity decreased
		}
	}

	tuple<array<vec3, 2>, array<vec3, 2>> Contact_resolver::apply_move(Contact& contact, Additional_contact_data& more_data) {
		array<vec3, 2> velocity_change = {k_vec3_zero, k_vec3_zero};
		array<vec3, 2> rotation_change = {k_vec3_zero, k_vec3_zero};

		//vec3 unit_impulse_local_contact = compute_frictionless_contact_local_unit_impulse(contact, more_data);
		vec3 unit_impulse_local_contact = compute_contact_local_unit_impulse_with_friction(contact, more_data);
		vec3 unit_impulse = more_data.contact_to_world_rotation * unit_impulse_local_contact;

		for (int i = 0; i < 2; i++) {
			float sign = -1 + 2 * i; // = i ? -1 : 1;
			auto& obj = *contact.objs[i];
			vec3 impulsive_torque = cross(more_data.relative_contact_positions[i], unit_impulse);
			rotation_change[i] = sign * obj.get_inverse_invertia_tensor_world() * impulsive_torque;
			velocity_change[i] = sign * unit_impulse * obj.get_inverse_mass();

#ifdef DEBUGGING
			float total_old_kinetic_energy = 0;
			for (int i = 0; i < 2; i++) {
				if (contact.objs[i]->get_inverse_mass() != 0) {
					total_old_kinetic_energy += contact.objs[i]->get_kinetic_energy();
				}
			}
#endif // DEBUGGING

			obj.add_velocity(velocity_change[i]);
			obj.add_angular_velocity(rotation_change[i]);
		
#ifdef DEBUGGING
			float total_new_kinetic_energy = 0;
			for (int i = 0; i < 2; i++) {
				if (contact.objs[i]->get_inverse_mass() != 0) {
					total_new_kinetic_energy += contact.objs[i]->get_kinetic_energy();
				}
			}
			//assert(total_new_kinetic_energy <= total_old_kinetic_energy);
#endif // DEBUGGING
		}

		return {velocity_change, rotation_change};
	}

	vec3 Contact_resolver::compute_frictionless_contact_local_unit_impulse(Contact& contact, Additional_contact_data& more_data) {
		vec3 unit_impulse_local_contact = k_vec3_zero;

		float delta_velocity = 0;
		for (int i = 0; i < 2; i++) {
			vec3 delta_vel_world = cross(more_data.relative_contact_positions[i], contact.normal);
			delta_vel_world = contact.objs[i]->get_inverse_invertia_tensor_world() * delta_vel_world;
			delta_vel_world = cross(delta_vel_world, more_data.relative_contact_positions[i]);

			delta_velocity += dot(delta_vel_world, contact.normal);
			delta_velocity += contact.objs[i]->get_inverse_mass();
		}

		// Calculate the required size of the unit_impulse
		unit_impulse_local_contact.y = more_data.desired_delta_velocity / delta_velocity;
		return unit_impulse_local_contact;
	}

	vec3 Contact_resolver::compute_contact_local_unit_impulse_with_friction(Contact& contact, Additional_contact_data& more_data) {
		float inverse_mass = 0;
		vec3 unit_impulse_local_contact = k_vec3_zero;
		// The equivalent of a cross product in matrices is multiplication
		// by a skew-symmetric matrix - we build the matrix for converting
		// between linear and angular quantities.
		mat3 delta_vel_world{0};
		for (int i = 0; i < 2; i++) {
			inverse_mass += contact.objs[i]->get_inverse_mass();
			mat3 impulse_to_torque = get_skew_symmetric(more_data.relative_contact_positions[i]);
			mat3 delta_vel_world_i = impulse_to_torque;
			delta_vel_world_i *= contact.objs[i]->get_inverse_invertia_tensor_world();
			delta_vel_world_i *= impulse_to_torque;
			delta_vel_world_i *= -1;
			delta_vel_world += delta_vel_world_i;
		}
		// Do a change of basis to convert into contact coordinates.
		mat3 contact_to_world_rotation = mat3(more_data.contact_to_world_rotation);
		mat3 delta_velocity = transpose(contact_to_world_rotation);
		delta_velocity *= delta_vel_world;
		delta_velocity *= contact_to_world_rotation;
		// Add in the linear velocity change.
		delta_velocity[0][0] += inverse_mass;
		delta_velocity[1][1] += inverse_mass;
		delta_velocity[2][2] += inverse_mass;
		// Invert to get the impulse needed per unit velocity.
		mat3 impulse_matrix = inverse(delta_velocity);
		// Find the target velocities to kill.
		vec3 vel_kill{-more_data.contact_local_velocity.x, more_data.desired_delta_velocity, -more_data.contact_local_velocity.z };
		// Find the impulse to kill target velocities.
		unit_impulse_local_contact = impulse_matrix * vel_kill;
		// Check for exceeding friction.
		float planar_impulse = sqrtf(unit_impulse_local_contact.x * unit_impulse_local_contact.x +
			unit_impulse_local_contact.z * unit_impulse_local_contact.z);
		if (planar_impulse > unit_impulse_local_contact.y * k_friction_coef)
		{
			// We need to use dynamic friction.
			unit_impulse_local_contact.x /= planar_impulse;
			unit_impulse_local_contact.z /= planar_impulse;
			unit_impulse_local_contact.y = delta_velocity[1][1] +
				delta_velocity[1][0] * k_friction_coef * unit_impulse_local_contact.x +
				delta_velocity[1][2] * k_friction_coef * unit_impulse_local_contact.z; // TODO e bine?
			unit_impulse_local_contact.y = more_data.desired_delta_velocity / unit_impulse_local_contact.y;
			unit_impulse_local_contact.x *= k_friction_coef * unit_impulse_local_contact.y;
			unit_impulse_local_contact.z *= k_friction_coef * unit_impulse_local_contact.y;
		}
		return unit_impulse_local_contact;
	}
}
