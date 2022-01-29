#include "Box.h"

#include <migine/Resource_manager.h>

using glm::vec3;
using glm::quat;

namespace migine {
	Box::Box(vec3 position, vec3 scale, quat rotation) :
		Has_mesh(get_mesh<Mesh_id::box>()), Has_shader(get_shader<Shader_id::vertex_normal>()), Has_transform(position, scale, rotation) {
	}
}
