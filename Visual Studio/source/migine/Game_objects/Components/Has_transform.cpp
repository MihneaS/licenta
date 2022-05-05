#include "Has_transform.h"

using glm::vec3;
using glm::quat;

namespace migine {
	Has_transform::Has_transform(vec3 position, vec3 scale, quat rotation) :
	transform(position, scale, rotation) {
	}
	Transform& Has_transform::get_transform() {
		return transform;
	}
}