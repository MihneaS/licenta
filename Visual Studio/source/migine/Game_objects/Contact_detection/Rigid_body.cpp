#include "Rigid_body.h"
#include <migine/constants.h>

using glm::vec3;
using glm::quat;

namespace migine {
	bool Rigid_body::integrate(float delta_time) {
		bool has_moved;
		if (speed == k_vec3_zero && angular_speed == quat()) {
			has_moved = false;
		} else {
			has_moved = true;
			transform.change_state_with_delta(speed * delta_time, vec3{1.0f}, angular_speed * delta_time);
		}
		return has_moved;
	}
}