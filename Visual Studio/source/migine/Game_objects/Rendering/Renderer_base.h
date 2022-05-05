#pragma once

#include <migine/Camera.h>
#include <migine/game_objects/components/Has_shader.h>
#include <migine/game_objects/components/Has_mesh.h>
#include <migine/game_objects/components/Has_transform_interface.h>

#include <stddef.h>

namespace migine {
	class RenderedObject;

	class Renderer_base : virtual public Has_shader, virtual public Has_mesh, virtual public Has_transform_interface {
	public:
		virtual void render(const Camera& camera) = 0;
	};
}