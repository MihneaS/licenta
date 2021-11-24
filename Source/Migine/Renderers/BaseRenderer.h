#pragma once

#include <Core/Engine.h>
#include <stddef.h>

namespace Migine {
	class RenderedObject;

	class BaseRenderer {
	public:
		// memory managed by someone else
		const Shader *shader;
		// memory managed by someone else
		RenderedObject *renderedObject;

		BaseRenderer(const Shader *shader, RenderedObject* renderedObject);
		
		virtual void Render(const EngineComponents::Camera*camera) = 0;
	};
}