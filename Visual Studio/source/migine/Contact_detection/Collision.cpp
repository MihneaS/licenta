#include "Collision.h"

using glm::vec3;

namespace migine {
	Collision::Collision(const Collider_base& obj1, const Collider_base& obj2, vec3 contact_point, vec3 normal, float penetration_depth) :
		obj1(obj1), obj2(obj2), contact_point(contact_point), normal(normal), penetration_depth(penetration_depth) {
	}
}