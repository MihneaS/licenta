#include "Sphere.h"

#include <migine/Resource_manager.h>

using glm::vec3;
using glm::quat;

using std::vector;

namespace migine {
	Sphere::Sphere(vec3 position, float diameter, quat rotation) :
		Has_mesh(get_mesh<Mesh_id::sphere>()), Has_shader(get_shader<Shader_id::vertex_normal>()), Has_transform(position, vec3({diameter, diameter, diameter}), rotation) {
	}
}