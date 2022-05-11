#pragma once
#include <migine/game_objects/Game_object.h>
#include <migine/game_objects/rendering/Color_renderer.h>
#include <migine/physics/Rigid_body.h>

namespace migine {
	class Debug_point : public Game_object, public Color_renderer, public Has_transform { // , public Rigid_body{
	public:
		explicit Debug_point(glm::vec3 position = { 0,0,0 }, float diameter = 0.05);
	};
}