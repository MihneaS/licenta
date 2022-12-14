#pragma once

#include <migine/game_objects/Game_object.h>
#include <migine/contact_detection/Box_collider.h>
#include <migine/game_objects/rendering/Simple_renderer.h>
#include <migine/physics/Rigid_body.h>

//#include <migine/game_objects/physics/get_initial_force_generators.h>

namespace migine {
	class Box_base : public Game_object, public Box_collider { //, public Rigid_body{
	public:
		explicit Box_base(glm::vec3 position = {0,0,0}, glm::vec3 scale = {1,1,1}, glm::quat rotation = glm::quat());
		void compute_inverse_inertia_tensor() override;
	};
}

