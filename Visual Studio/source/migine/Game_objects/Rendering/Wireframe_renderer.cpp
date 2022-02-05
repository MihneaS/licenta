#include "Wireframe_renderer.h"

using glm::vec3;
using glm::quat;

namespace migine {

	Wireframe_renderer::Wireframe_renderer(const Shader& shader, const Mesh& mesh, vec3 position, vec3 scale, quat rotation, vec3 color, float line_width) :
		Has_shader(shader), Has_mesh(mesh), Has_transform(position, scale, rotation), color(color), line_width(line_width) {
	}

	void Wireframe_renderer::render(const Camera& camera) {
		glLineWidth(line_width);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform.get_model()));
		glUniform3fv(shader.GetUniformLocation("color"), 1, glm::value_ptr(color));
		mesh.Render();
	}
}