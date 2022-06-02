#include "Contact.h"

#include <migine/utils.h>

using glm::vec3;
using glm::mat4;
using glm::quat;
using glm::translate;
using glm::length;

using gsl::not_null;

namespace migine {
	Contact::Contact(not_null<Collider_base*> obj0, not_null<Collider_base*> obj1, vec3 contact_point, vec3 normal, float penetration_depth) :
		objs{obj0, obj1}, contact_point(contact_point), normal(normal), penetration_depth(penetration_depth) {
		assert(is_equal_aprox(length(normal), 1)); // DEMO1
		if (std::abs(glm::dot(normal, vec3{0,1,0})) < 1.0f / 1.41f) {
			// TODO delete this for
			int i = 0;
			assert(true);
			i++;
		}
	}

	quat Contact::get_rotation_to_contact_basis() const {
		vec3 old_x;
		if (normal.x > normal.z) {
			old_x = k_right;
		} else {
			old_x = k_back;
		}

		return change_rotation(old_x, k_right, normal, k_up);
	}
}