#pragma once

#include "Mesh.h"

namespace migine {
	class Has_mesh {
	public:
		Has_mesh(const Mesh& mesh);

		const Mesh& mesh;
	};
}