#pragma once

#include <migine/game_objects/Game_object.h>
#include <migine/contact_detection/Sphere_collider.h>
#include <migine/game_objects/rendering/Simple_renderer.h>
#include <migine/physics/Rigid_body.h>

//#include <migine/game_objects/physics/get_initial_force_generators.h>

namespace migine {
	class Sphere : public Game_object, public Sphere_collider, public Simple_renderer { // , public Rigid_body{
	public:
		Sphere(glm::vec3 position = { 0,0,0 }, float diameter = 1, glm::quat rotation = glm::quat());
		void compute_inverse_inertia_tensor() override;
	};
}