#pragma once

#include <Core/Engine.h>
#include <stddef.h>

namespace Migine {
	class RenderedObject;

	class BaseRenderer {
	public:
		const Shader *shader;
		RenderedObject *renderedObject;

		BaseRenderer(const Shader *shader, RenderedObject* renderedObject);
		
		virtual void Render(const EngineComponents::Camera*camera) = 0;
	};
}