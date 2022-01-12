#pragma once

#include <Migine/Renderers/BaseRenderer.h>
#include <Migine/constants.h>

namespace Migine {
	class WireframeRenderer final : public BaseRenderer {
	public:
		glm::vec3 color;
		float lineWidth;

		WireframeRenderer(Shader* shader, RenderedObject* renderedObject,
				glm::vec3 color = kDefaultColor, float lineWidth = kDefaultLineWidth);

		void Render(const EngineComponents::Camera* camera) override;
	};
}