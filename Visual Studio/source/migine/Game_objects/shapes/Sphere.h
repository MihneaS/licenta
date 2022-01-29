#pragma once

#include <migine/Game_objects/Game_object.h>
#include <migine/Game_objects/Contact_detection/Sphere_collider.h>
#include <migine/Game_objects/Rendering/Simple_renderer.h>
#include <migine/Game_objects/Contact_detection/Rigid_body.h>

namespace migine {
	class Sphere : public Game_object, public Sphere_collider, public Simple_renderer { // , public Rigid_body{
	public:
		Sphere(glm::vec3 position = { 0,0,0 }, float diameter = 1, glm::quat rotation = glm::quat());
		~Sphere() override = default;
	};
}