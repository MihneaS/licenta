#pragma once

#include <migine/define.h>
#include <migine/Game_objects/Components/Has_name.h>

namespace migine {
	class Game_object 
#ifdef DEBUGGING
		: virtual public Has_name
#endif // DEBUGGING
	{
	public:
		Game_object() = default;
		virtual ~Game_object() = default;

		virtual void init();
		virtual void update();
	};
}