#pragma once
#include "Transform.h"

namespace migine {
	class Has_transform {
	public:
		Has_transform(glm::vec3 position = {0, 0, 0}, glm::vec3 scale =  {1, 1, 1}, glm::quat rotation = glm::quat());

		Transform transform;
	};
}