#pragma once

#include <migine/game_objects/rendering/Renderer_base.h>
#include <migine/constants.h>

namespace migine {
	class Wireframe_renderer : public Renderer_base {
	public:
		Wireframe_renderer(const Shader& shader, const Mesh& mesh, glm::vec3 position = {0, 0, 0}, glm::vec3 scale = {1, 1, 1}, glm::quat rotation = glm::quat(),
				glm::vec3 color = k_default_color, float lineWidth = k_default_line_width);

		void render(const Camera& camera) override;

		glm::vec3 color;
		float line_width;

	protected:
		Wireframe_renderer();
	};
}