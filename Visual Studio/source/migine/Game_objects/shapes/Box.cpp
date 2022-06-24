#include "Box.h"

#include <migine/Resource_manager.h>
#include <migine/scenes/Scene_base.h>

using glm::vec3;
using glm::quat;
using glm::mat3;

namespace migine {
	Box::Box(vec3 position, vec3 scale, quat rotation) :
		Box_base(position, scale, rotation), Has_mesh(get_mesh<Mesh_id::box>()),
		Has_shader(get_shader<Shader_id::vertex_normal>()), Has_spcon_transform(position, scale, rotation) {
	}

	std::unique_ptr<Game_object> Box::self_unregister(Scene_base& scene) {
		return scene.unregister_game_object(this);
	}
}
