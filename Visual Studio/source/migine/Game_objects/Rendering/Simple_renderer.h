#pragma once

#include "Renderer_base.h"

namespace migine {
	class Simple_renderer : public Renderer_base {
	public:
		Simple_renderer(const Shader& shader, const Mesh& mesh, glm::vec3 position = {0, 0, 0}, glm::vec3 scale = {1, 1, 1}, glm::quat rotation = glm::quat());

		void render(const Camera& camera) override;

	protected:
		Simple_renderer();
	};
}