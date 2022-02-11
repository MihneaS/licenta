#pragma once

#include <glm/glm.hpp>
namespace migine {
	class Collider_base;

	struct Collision {
		Collision(const Collider_base& obj1, const Collider_base& obj2, glm::vec3 contact_point, glm::vec3 normal, float penetration_depth);
		const Collider_base& obj1;
		const Collider_base& obj2;
		glm::vec3 contact_point;
		glm::vec3 normal;
		float penetration_depth;
	};
}