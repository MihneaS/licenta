#pragma once
#include "Transform.h"
#include <migine/game_objects/components/Has_transform_interface.h>

namespace migine {
	class BVH;
	class Has_transform : public virtual Has_transform_interface {
		friend class BVH;
	public:
		Has_transform(glm::vec3 position = {0, 0, 0}, glm::vec3 scale =  {1, 1, 1}, glm::quat rotation = glm::quat());
		Has_transform(const Has_transform& other) = default;

		Transform& get_transform() override;

		Transform transform;
	};
}