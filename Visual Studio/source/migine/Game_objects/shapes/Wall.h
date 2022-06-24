#pragma once

#include <migine/game_objects/shapes/Box_base.h>
#include <migine/game_objects/rendering/Color_renderer.h>

//#include <migine/game_objects/physics/get_initial_force_generators.h>

namespace migine {
	class Wall : public Box_base, public Color_renderer { //, public Rigid_body{
	public:
		explicit Wall(glm::vec3 position = {0,0,0}, glm::vec3 scale = {1,1,1}, glm::quat rotation = glm::quat(), glm::vec3 color = k_default_wall_color);

		std::tuple<glm::vec3, glm::vec3> provide_fat_aabb_parameters() const override;
		void render(const Camera& camera) override;
		std::unique_ptr<Game_object> self_unregister(Scene_base& scene) override;
	};
}

