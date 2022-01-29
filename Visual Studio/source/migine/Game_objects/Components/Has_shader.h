#pragma once

#include "Shader.h"

namespace migine {
	class Has_shader {
	public:
		Has_shader(const Shader& shader);

		const Shader& shader;
	};
}