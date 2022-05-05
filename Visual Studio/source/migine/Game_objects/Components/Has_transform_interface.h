#pragma once

#include <include/glm.h>

namespace migine {
	class Transform;
	class Has_transform_interface {
	public:
		virtual Transform& get_transform() = 0;
	};
}