#include "Spcon_transform.h"
#include <Migine/scenes/current_scene.h>

using glm::vec3;
using glm::quat;

namespace migine{
	Spcon_transform::Spcon_transform(vec3 position, vec3 scale, quat orientation) :
		Transform(position, scale, orientation) {
	}

	void migine::Spcon_transform::internal_update() {
		Transform::internal_update();
		if (bvh_node) {
			get_current_scene().bvh.mark_dirty_node(bvh_node);
		}
	}
}
