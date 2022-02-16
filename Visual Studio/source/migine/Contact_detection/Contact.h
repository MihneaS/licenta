#pragma once

#include <glm/glm.hpp>
#include <gsl/gsl>

#include <migine/utils.h>

#include <tuple>
#include <array>

namespace migine {
	class Collider_base;

	struct Contact {
		Contact(gsl::not_null<Collider_base*> obj0, gsl::not_null<Collider_base*> obj1, glm::vec3 contact_point, glm::vec3 normal, float penetration_depth);

		glm::quat get_rotation_to_contact_basis() const;

		std::array<gsl::not_null<Collider_base*>, 2> objs;
		glm::vec3 contact_point;
		glm::vec3 normal;
		float penetration_depth;
	};
}