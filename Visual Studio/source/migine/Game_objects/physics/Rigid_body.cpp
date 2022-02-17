#include "Rigid_body.h"
#include <migine/constants.h>
#include <migine/utils.h>

using glm::vec3;
using glm::quat;
using glm::mat3;
using glm::mat4;
using glm::inverse;
using glm::cross;

namespace migine {
	bool Rigid_body::integrate(float delta_time) {
		bool has_moved = false;

		// Calculate linear acceleration from force inputs.
		vec3 acceleration = constant_acceleration;
		acceleration += force_accumulator * get_inverse_mass();
		// Calculate angular acceleration from torque inputs.
		vec3 angular_acceleration = inverse_inertia_tensor_world * torque_accumulator;
		// Adjust velocities
		// Update linear velocity from both acceleration and impulse.
		last_frame_acceleration = acceleration;
		velocity += last_frame_acceleration * delta_time;
		// Update angular velocity from both acceleration and impulse.
		angular_velocity += angular_acceleration * delta_time;
		// Impose drag.
		velocity *= pow(linear_damping, delta_time);
		angular_velocity *= pow(angular_damping, delta_time);
		if (velocity != k_vec3_zero || angular_velocity != k_vec3_zero) {
			has_moved = true;
			// Adjust position and orientation
			transform.change_position_with_delta(velocity * delta_time);
			transform.change_orientation_with_delta(angular_velocity * delta_time);
			// Normalize the orientation, and update the matrices with the new position and orientation.
			calculate_derived_data();
		}
		// Clear accumulators.
		clear_accumulators();

		return has_moved;
	}

	float Rigid_body::get_mass() const {
		return 1.0f / inverse_mass;
	}

	void Rigid_body::set_mass(float mass) {
		inverse_mass = 1.0f / mass;
	}

	float Rigid_body::get_inverse_mass() const {
		return inverse_mass;
	}

	void Rigid_body::set_inverse_mass(float inverse_mass) {
		this->inverse_mass = inverse_mass;
	}

	vec3 Rigid_body::get_constant_acceleration() const {
		return constant_acceleration;
	}

	const glm::mat3& Rigid_body::get_inverse_invertia_tensor() const {
		return inverse_inertia_tensor;
	}

	void Rigid_body::set_inertia_tensor(const mat3& inerta_tensor) {
		inverse_inertia_tensor = inverse(inerta_tensor);
	}

	void Rigid_body::set_inverse_inertia_tensor(const glm::mat3& inverse_inertia_tensor) {
		this->inverse_inertia_tensor = inverse_inertia_tensor;
	}

	void Rigid_body::compute_inverse_inertia_tensor_world() { // TODO NU FOLOSI! matricile ar trebui transpuse!
			mat3& iit_world = inverse_inertia_tensor_world;
			const mat3& iit_body = inverse_inertia_tensor;
			const mat4& rot_mat = transform.get_model();
			float t04 = rot_mat[0][0] * iit_body[0][0] +	       
			            rot_mat[0][1] * iit_body[1][0] +
			            rot_mat[0][2] * iit_body[2][0];
			float t09 = rot_mat[0][0] * iit_body[0][1] +
			            rot_mat[0][1] * iit_body[1][1] +
			            rot_mat[0][2] * iit_body[2][1];
			float t14 = rot_mat[0][0] * iit_body[0][2] +
			            rot_mat[0][1] * iit_body[1][2] +
			            rot_mat[0][2] * iit_body[2][2];
			float t28 = rot_mat[1][0] * iit_body[0][0] +
			            rot_mat[1][1] * iit_body[1][0] +
			            rot_mat[1][2] * iit_body[2][0];
			float t33 = rot_mat[1][0] * iit_body[0][1] +
			            rot_mat[1][1] * iit_body[1][1] +
			            rot_mat[1][2] * iit_body[2][1];
			float t38 = rot_mat[1][0] * iit_body[0][2] +
			            rot_mat[1][1] * iit_body[1][2] +
			            rot_mat[1][2] * iit_body[2][2];
			float t52 = rot_mat[2][0] * iit_body[0][0] +
			            rot_mat[2][1] * iit_body[1][0] +
			            rot_mat[2][2] * iit_body[2][0];
			float t57 = rot_mat[2][0] * iit_body[0][1] +
			            rot_mat[2][1] * iit_body[1][1] +
			            rot_mat[2][2] * iit_body[2][1];
			float t62 = rot_mat[2][0] * iit_body[0][2] +
			            rot_mat[2][1] * iit_body[1][2] +
			            rot_mat[2][2] * iit_body[2][2];
			iit_world[0][0] = t04 * rot_mat[0][0] +
			                  t09 * rot_mat[0][1] +
			                  t14 * rot_mat[0][2];
			iit_world[0][1] = t04 * rot_mat[1][0] +
			                  t09 * rot_mat[1][1] +
			                  t14 * rot_mat[1][2];
			iit_world[0][2] = t04 * rot_mat[2][0] +
			                  t09 * rot_mat[2][1] +
			                  t14 * rot_mat[2][2];
			iit_world[1][0] = t28 * rot_mat[0][0] +
			                  t33 * rot_mat[0][1] +
			                  t38 * rot_mat[0][2];
			iit_world[1][1] = t28 * rot_mat[1][0] +
			                  t33 * rot_mat[1][1] +
			                  t38 * rot_mat[1][2];
			iit_world[1][2] = t28 * rot_mat[2][0] +
			                  t33 * rot_mat[2][1] +
			                  t38 * rot_mat[2][2];
			iit_world[2][0] = t52 * rot_mat[0][0] +
			                  t57 * rot_mat[0][1] +
			                  t62 * rot_mat[0][2];
			iit_world[2][1] = t52 * rot_mat[1][0] +
			                  t57 * rot_mat[1][1] +
			                  t62 * rot_mat[1][2];
			iit_world[2][2] = t52 * rot_mat[2][0] +
			                  t57 * rot_mat[2][1] +
			                  t62 * rot_mat[2][2];
	}

	vec3 Rigid_body::get_velocity() const {
		return velocity;
	}

	void Rigid_body::add_velocity(vec3 velocity) {
		this->velocity += velocity;
	}

	vec3 Rigid_body::get_angular_velocity() const {
		return angular_velocity;
	}

	void Rigid_body::add_angular_velocity(vec3 rotation) {
		this->angular_velocity += rotation;
	}

	vec3 Rigid_body::get_last_frame_acceleration() const {
		return last_frame_acceleration;
	}

	void Rigid_body::add_force(vec3 force) {
		force_accumulator += force;
	}

	void Rigid_body::add_force_at_point(vec3 force, vec3 point) {
		add_force(force);
		add_torque(cross(force, point - transform.get_world_position())); // TODO e bine?
	}

	void Rigid_body::add_force_at_body_point(vec3 force, vec3 point) {
		add_force_at_point(force, transform.get_point_in_world_space(point));
	}

	void Rigid_body::add_torque(vec3 torque) {
		torque_accumulator += torque;
	}

	void Rigid_body::clear_force_accum() {
		force_accumulator = k_vec3_zero;
	}

	void Rigid_body::clear_torque_accum() {
		torque_accumulator = k_vec3_zero;
	}

	void Rigid_body::clear_accumulators() {
		clear_force_accum();
		clear_torque_accum();
	}

	void Rigid_body::calculate_derived_data() {
		//_transform_inertia_tensor(inverse_inertia_tensor_world, inverse_inertia_tensor, transform.get_model());
		compute_inverse_inertia_tensor_world();
		transform.internal_update();
	}
}