#pragma once

#include <migine/Game_objects/Components/Has_transform.h>

#include <Core/Engine.h>

namespace migine {
	class Rigid_body  : virtual public Has_transform {
	public:
		bool integrate(float delta_time);

		glm::vec3 speed = {0,0,0};
		glm::quat angular_speed = glm::quat();
	};
}