#pragma once

#include "Transform.h"
#include <migine/contact_detection/BVH.h>

namespace migine {
	class Spcon_transform : public Transform {
	public:
		explicit Spcon_transform(glm::vec3 position = k_vec3_zero, glm::vec3 scale = { 1, 1, 1 }, glm::quat orientation = glm::quat());
		void internal_update() override;
	private:
		BVH::Node* bvh_node = nullptr;
	};
}