#include "Box_base.h"

#include <migine/Resource_manager.h>

using glm::vec3;
using glm::quat;
using glm::mat3;

namespace migine {
	Box_base::Box_base(vec3 position, vec3 scale, quat rotation) :
		Has_mesh(get_mesh<Mesh_id::box>()), Has_spcon_transform(position, scale, rotation) {
		compute_inverse_inertia_tensor();
	}

	void Box_base::compute_inverse_inertia_tensor() {
		auto scale = transform.get_scale();
		float dx2 = scale.x * scale.x;
		float dy2 = scale.y * scale.y;
		float dz2 = scale.z * scale.z;
		float im_times_12 = get_inverse_mass() * 12;
		set_inverse_inertia_tensor(mat3{{im_times_12 / (dy2 + dz2), 0, 0},{0, im_times_12 / (dx2 + dz2), 0},{0, 0, im_times_12 / (dx2 + dy2)}});
		compute_inverse_inertia_tensor_world();
	}
}