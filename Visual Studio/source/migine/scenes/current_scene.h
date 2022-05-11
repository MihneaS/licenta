#pragma once

#include <migine/scenes/Scene_base.h>

namespace migine {
	Scene_base& get_current_scene();

	template<class T>
	T& get_specific_scene() {
		//assert(std::is_base_of<Scene_base, T>())
		return dynamic_cast<T&>(get_current_scene());
	}
}