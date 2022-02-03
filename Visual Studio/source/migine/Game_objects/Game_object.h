#pragma once

#include <migine/define.h>
#include <migine/game_objects/components/Has_name.h>

namespace migine {
	class Game_object 
#ifdef DEBUGGING
		: virtual public Has_name
#endif // DEBUGGING
	{
	public:
		Game_object() = default;
		virtual ~Game_object() = default; // TODO replace with = 0

		virtual void init();
		virtual void update();
	};
}