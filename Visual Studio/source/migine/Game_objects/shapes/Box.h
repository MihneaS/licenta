#pragma once

#include <migine/Game_objects/Game_object.h>
#include <migine/Game_objects/Contact_detection/Box_collider.h>
#include <migine/Game_objects/Rendering/Simple_renderer.h>
#include <migine/Game_objects/Contact_detection/Rigid_body.h>

namespace migine {
	class Box : public Game_object, public Box_collider, public Simple_renderer { //, public Rigid_body{
	public:
		Box(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
	};
}