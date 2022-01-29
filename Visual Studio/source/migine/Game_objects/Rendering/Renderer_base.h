#pragma once

#include <migine/Camera.h>
#include <migine/Game_objects/Components/Has_shader.h>
#include <migine/Game_objects/Components/Has_mesh.h>
#include <migine/Game_objects/Components/Has_transform.h>

#include <stddef.h>

namespace migine {
	class RenderedObject;

	class Renderer_base : virtual public Has_shader, virtual public Has_mesh, virtual public Has_transform {
	public:
		virtual void render(const Camera& camera) = 0;
	};
}