#pragma once

#include <migine/game_objects/physics/Rigid_body.h>
#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>

#include <vector>

namespace migine {
	template<class T>
	std::vector<std::unique_ptr<Force_generator_base>> get_initial_force_generators() {
		static_assert(std::is_base_of<Rigid_body, T>());
		std::vector<std::unique_ptr<Force_generator_base>> ret(2);
		ret.push_back(make_unique<Gravity_generator>());
		ret.push_back(make_unique<Drag_generator>(0.5f, 0.5f));
		return ret;
	}
}