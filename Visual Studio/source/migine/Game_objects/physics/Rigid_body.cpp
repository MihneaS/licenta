#include "Rigid_body.h"
#include <migine/constants.h>

using glm::vec3;
using glm::quat;

namespace migine {
	bool Rigid_body::integrate(float delta_time) {
		bool has_moved;
		if (velocity == k_vec3_zero && angular_velocity == quat()) {
			has_moved = false;
		} else {
			has_moved = true;
			transform.change_state_with_delta(velocity * delta_time, vec3{1.0f}, angular_velocity * delta_time);
		}
		return has_moved;
	}

	float Rigid_body::get_mass() {
		return 1.0f / inverse_mass;
	}

	void Rigid_body::set_mass(float mass) {
		inverse_mass = 1.0f / mass;
	}

	float Rigid_body::get_inverse_mass() {
		return inverse_mass;
	}

	void Rigid_body::set_inverse_mass(float inverse_mass) {
		this->inverse_mass = inverse_mass;
	}

	glm::vec3 Rigid_body::get_velocity() {
		return velocity;
	}

	glm::quat Rigid_body::get_angular_velocity() {
		return angular_velocity;
	}

	void Rigid_body::add_force(glm::vec3 force) {
		force_acumulator += force;
	}

	void Rigid_body::reset_forces() {
		force_acumulator = {0, 0, 0};
	}


}