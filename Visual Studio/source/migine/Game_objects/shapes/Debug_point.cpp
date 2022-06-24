#include "Debug_point.h"

#include <migine/Resource_manager.h>
#include <migine/scenes/Scene_base.h>

using glm::vec3;
using glm::quat;
using glm::mat3;

using std::vector;

namespace migine {
	Debug_point::Debug_point(vec3 position, float diameter) :
		Has_mesh(get_mesh<Mesh_id::sphere>()), Has_shader(get_shader<Shader_id::color>()), Has_transform(position, vec3({ diameter, diameter, diameter }), quat()) {
	}
	std::unique_ptr<Game_object> Debug_point::self_unregister(Scene_base& scene) {
		return scene.unregister_game_object(this);
	}
}