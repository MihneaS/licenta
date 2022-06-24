#pragma once

#include <migine/game_objects/shapes/Box_base.h>
#include <migine/game_objects/rendering/Simple_renderer.h>

//#include <migine/game_objects/physics/get_initial_force_generators.h>

namespace migine {
	class Box : public Box_base, public Simple_renderer { //, public Rigid_body{
	public:
		explicit Box(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		std::unique_ptr<Game_object> self_unregister(Scene_base& scene) override;
	};
}

