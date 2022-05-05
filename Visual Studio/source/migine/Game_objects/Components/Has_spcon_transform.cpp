#include "Has_spcon_transform.h"

using glm::vec3;
using glm::quat;

namespace migine {
	Has_spcon_transform::Has_spcon_transform(vec3 position, vec3 scale, quat rotation) :
		transform(position, scale, rotation) {
	}
	Transform& Has_spcon_transform::get_transform() {
		return transform;
	}
}