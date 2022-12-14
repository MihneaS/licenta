#include "Sphere.h"

#include <migine/Resource_manager.h>
#include <migine/scenes/Scene_base.h>

using glm::vec3;
using glm::quat;
using glm::mat3;

using std::vector;

namespace migine {
	Sphere::Sphere(vec3 position, float diameter, quat rotation) :
		Has_mesh(get_mesh<Mesh_id::sphere>()), Has_shader(get_shader<Shader_id::vertex_normal>()), Has_spcon_transform(position, vec3({diameter, diameter, diameter}), rotation) {
		compute_inverse_inertia_tensor();
	}
	void Sphere::compute_inverse_inertia_tensor() {
		float d = transform.get_scale().x;
		float im_10_div_d2 = get_inverse_mass()*10 / (d*d);
		set_inverse_inertia_tensor(mat3() * im_10_div_d2);
		compute_inverse_inertia_tensor_world();
	}
	std::unique_ptr<Game_object> Sphere::self_unregister(Scene_base& scene) {
		return scene.unregister_game_object(this);
	}
}