#include "Rigid_body.h"
#include <migine/constants.h>
#include <migine/utils.h>
#include <migine/contact_detection/Collider_base.h>

using glm::vec3;
using glm::quat;
using glm::mat3;
using glm::mat4;
using glm::inverse;
using glm::transpose;
using glm::cross;
using glm::dot;

namespace migine {
	void Rigid_body::integrate(float delta_time) {
		if (get_inverse_mass() == 0) {
			last_frame_acceleration = k_vec3_zero;
			return;
		}

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
		
		assert(is_finite(velocity));
		assert(is_finite(angular_velocity));

		float bias = powf(k_motion_base_bias, delta_time);
		float current_motion = dot(velocity, velocity) + dot(angular_velocity, angular_velocity);
		motion = bias * motion + (1 - bias) * current_motion;
		if (motion > k_max_motion) {
			motion = k_max_motion;
			//} else if (motion < k_sleep_epsilon && dynamic_cast<Collider_base*>(this)->is_in_contact_with_other_static()) {
		} else if (motion < k_sleep_epsilon) {
			motion = 0;
			velocity = k_vec3_zero;
			angular_velocity = k_vec3_zero;
			set_asleep(true);
		}
		
		if (!is_asleep()) {
			// Adjust position and orientation
			transform.change_position_with_delta(velocity * delta_time);
			transform.change_orientation_with_delta(angular_velocity * delta_time);
			// Normalize the orientation, and update the matrices with the new position and orientation.
			calculate_derived_data();
		}

		// Clear accumulators.
		clear_accumulators();
	}

	float Rigid_body::get_mass() const {
		return 1.0f / inverse_mass;
	}

	void Rigid_body::set_mass(float mass) {
		inverse_mass = 1.0f / mass;
		compute_inverse_inertia_tensor();
	}

	float Rigid_body::get_inverse_mass() const {
		return inverse_mass;
	}

	void Rigid_body::set_inverse_mass(float inverse_mass) {
		this->inverse_mass = inverse_mass;
		if (inverse_mass == 0) {
			motion = 0;
			velocity = k_vec3_zero;
			angular_velocity = k_vec3_zero;
			set_asleep(true);
		}
		compute_inverse_inertia_tensor();
	}

	vec3 Rigid_body::get_constant_acceleration() const {
		return constant_acceleration;
	}

	mat3 Rigid_body::get_inertia_tensor_world() const {
		return inverse(inverse_inertia_tensor_world);
	}

	const glm::mat3& Rigid_body::get_inverse_invertia_tensor() const {
		return inverse_inertia_tensor;
	}

	const mat3& Rigid_body::get_inverse_invertia_tensor_world() const {
		return inverse_inertia_tensor_world;
	}

	void Rigid_body::set_inertia_tensor(const mat3& inerta_tensor) {
		inverse_inertia_tensor = inverse(inerta_tensor);
	}

	void Rigid_body::set_inverse_inertia_tensor(const glm::mat3& inverse_inertia_tensor) {
		this->inverse_inertia_tensor = inverse_inertia_tensor;
	}

	void Rigid_body::compute_inverse_inertia_tensor_world() {
		//mat3 inv = inverse(transform.get_model());
		
		//mat3 inv = mat3(conjugate(transform.get_orientation()));
		//inverse_inertia_tensor_world = transpose(inv) * get_inverse_invertia_tensor() * inv;
		
		//mat4 inv = inverse(transform.get_model());
		//inverse_inertia_tensor_world = transpose(inv) * mat4(get_inverse_invertia_tensor()) * inv;
		//inverse_inertia_tensor_world = inv * mat4(get_inverse_invertia_tensor()) * transpose(inv);
		mat3 to_world_rotation = mat3(transform.get_orientation()); // TODO what about scaling? is it included in inverse inertia tensor calculation?
		inverse_inertia_tensor_world = to_world_rotation * inverse_inertia_tensor * transpose(to_world_rotation);
	}

	vec3 Rigid_body::get_velocity() const {
		return velocity;
	}

	void Rigid_body::add_velocity(vec3 velocity) {
		//assert(length(velocity) < 24);
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

	float Rigid_body::get_kinetic_energy() const {
		if (inverse_mass == 0) {
			return std::numeric_limits<float>::infinity();
		}
		float v_len = length(velocity);
		float ang_v_len = length(angular_velocity); // TODO e bine? poate trb * k_deg_to_rad
		float angular_kinetic_energy = 0;
		if (ang_v_len != 0) {
			float rotation_inertia_around_rotating_axis = 0;

			vec3 rotating_axis = angular_velocity / ang_v_len;

			// based on RotationBetweenVectors from https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
			// and rotating matrices from "Game Physics Engine Development"

			// I am bassically constructing a rotating matrix from rotating_axis to OX,
			// changing the coordinates of the inertia tensor from world coordinates to some coordinates where OX is the rotating axis
			// new_I = rot_mat * I_world * rot_mat^-1
			// and because for rotation matrices rot_mat^-1 = transpose(rot_mat)
			// new_I = rot*mat * I_world * transpose(rot_mat)
			// Therefore rotating inertia with respect to the rotating axis is new_I[0][0]
			float cos_theta = dot(rotating_axis, {1,0,0});
			if (cos_theta < -1 + k_float_epsilon) {
				rotation_inertia_around_rotating_axis = get_inertia_tensor_world()[0][0];
			} else {
				vec3 rot_axis = cross(rotating_axis, {1,0,0});
				float sin_theta = sqrtf(1 - cos_theta * cos_theta);
				float t_theta = 1 - cos_theta;
				float tx = t_theta * rot_axis.x;
				vec3 rot_mat_0{
						tx * rot_axis.x + cos_theta,
						tx * rot_axis.y + sin_theta * rot_axis.z,
						tx * rot_axis.z - sin_theta * rot_axis.y
				};
				mat3 inertia_tensor_world = get_inertia_tensor_world();
				//for (int i = 0; i < 3; i++) {
				//	float tmp = 0;
				//	for (int j = 0; j < 3; j++) {
				//		for (int k = 0; k < 3; k++) {
				//			tmp += rot_mat_0[k] * inertia_tensor_world[k][j]; // indices are right
				//		}
				//	}
				//	rotation_inertia_around_rotating_axis += rot_mat_0[i] * tmp;
				//}
				rotation_inertia_around_rotating_axis =
					rot_mat_0[0] * (rot_mat_0[0] * inertia_tensor_world[0][0] +
									rot_mat_0[1] * inertia_tensor_world[1][0] +
									rot_mat_0[2] * inertia_tensor_world[2][0]) +
					rot_mat_0[1] * (rot_mat_0[0] * inertia_tensor_world[0][1] +
									rot_mat_0[1] * inertia_tensor_world[1][1] +
									rot_mat_0[2] * inertia_tensor_world[2][1]) +
					rot_mat_0[2] * (rot_mat_0[0] * inertia_tensor_world[0][2] +
									rot_mat_0[1] * inertia_tensor_world[1][2] +
									rot_mat_0[2] * inertia_tensor_world[2][2]);
			}
			angular_kinetic_energy = rotation_inertia_around_rotating_axis * ang_v_len * ang_v_len * 0.5;
		}
		return (get_mass() * v_len * v_len) * 0.5f + angular_kinetic_energy;
	}

	bool Rigid_body::is_asleep() const {
		return asleep;
	}

	void Rigid_body::set_asleep(bool new_state) {
		asleep = new_state;
	}

	void Rigid_body::awake() {
		set_asleep(false);
		set_motion(k_max_motion);
	}

	void Rigid_body::stop_motion() {
		velocity = k_vec3_zero;
		angular_velocity = k_vec3_zero;
		last_frame_acceleration = k_vec3_zero;
		force_accumulator = k_vec3_zero; // e necesar?
		torque_accumulator = k_vec3_zero; // e necesar?
	}

	void Rigid_body::set_motion(float new_value) {
		motion = new_value;
	}

	std::vector<std::unique_ptr<Force_generator_base>>& Rigid_body::get_default_fs_gen() {
		return default_fs_gen;
	}
}
