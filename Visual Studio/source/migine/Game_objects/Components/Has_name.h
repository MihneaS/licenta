#pragma once
#include <migine/define.h>
#ifdef DEBUGGING
#include <string>

namespace migine {
	class Has_name {
	public:
		std::string name;
	};
}
#endif // DEBUGGING