#include "Wall.h"

#include <migine/Resource_manager.h>
#include <migine/scenes/current_scene.h>

using glm::vec3;
using glm::quat;
using glm::mat3;

namespace migine {
	Wall::Wall(vec3 position, vec3 scale, quat rotation, vec3 color) :
		Box_base(position, scale, rotation), Has_mesh(get_mesh<Mesh_id::box>()), Color_renderer(color),
		Has_shader(get_shader<Shader_id::color>()), Has_spcon_transform(position, scale, rotation) {
	}
	std::tuple<glm::vec3, glm::vec3> Wall::provide_fat_aabb_parameters() const {
		return provide_slim_aabb_parameters();
	}

	void Wall::render(const Camera& camera) {
		if (get_current_scene().see_walls) {
			Color_renderer::render(camera);
		}
	}

	std::unique_ptr<Game_object> Wall::self_unregister(Scene_base& scene) {
		return scene.unregister_game_object(this);
	}
}
