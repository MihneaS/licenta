#pragma once

#include "Renderer_base.h"

namespace migine {
	class Sphere;

	class Sphere_renderer : public Renderer_base {
	public:
		Sphere_renderer(const Shader& shader, const Mesh& mesh, const Sphere& sphere, glm::vec3 position = {0, 0, 0}, glm::vec3 scale = {1, 1, 1}, glm::quat rotation = glm::quat());

		void render(const Camera& camera) override;

	protected:
		Sphere_renderer(const Sphere& sphere);

	private:
		const Sphere& sphere;
	};
}