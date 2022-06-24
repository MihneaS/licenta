#pragma once

#include <migine/define.h>
#include <migine/game_objects/components/Has_name.h>
#include <migine/game_objects/components/Has_transform_interface.h>

namespace migine {
	class Scene_base;
	class Game_object : virtual public Has_transform_interface
#ifdef DEBUGGING
		, virtual public Has_name
#endif // DEBUGGING
	{
	public:
		Game_object() = default;
		virtual ~Game_object() = default; // TODO replace with = 0

		virtual void init();
		virtual void update();

		virtual std::unique_ptr<Game_object> self_unregister(Scene_base& scene) = 0;
	};
}