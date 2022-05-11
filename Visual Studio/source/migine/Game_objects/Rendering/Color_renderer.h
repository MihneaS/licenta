#pragma once

#include "Renderer_base.h"
#include <migine/constants.h>

namespace migine {
	class Color_renderer : public Renderer_base {
	public:
		Color_renderer(const Shader& shader, const Mesh& mesh, glm::vec3 position = { 0, 0, 0 }, glm::vec3 scale = { 1, 1, 1 }, glm::quat rotation = glm::quat(), glm::vec3 color = k_default_color);

		virtual void render(const Camera& camera) override;

	protected:
		Color_renderer();

	private:
		glm::vec3 color;
	};
}