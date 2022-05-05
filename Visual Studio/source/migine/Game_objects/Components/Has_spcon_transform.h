#pragma once
#include "Spcon_transform.h"
#include <migine/game_objects/components/Has_transform_interface.h>

namespace migine {
	class Has_spcon_transform : public virtual Has_transform_interface {
	public:
		Has_spcon_transform(glm::vec3 position = { 0, 0, 0 }, glm::vec3 scale = { 1, 1, 1 }, glm::quat rotation = glm::quat());

		Transform& get_transform() override;

		Spcon_transform transform;
	};
}