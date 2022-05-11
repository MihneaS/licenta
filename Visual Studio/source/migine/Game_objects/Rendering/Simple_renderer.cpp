#include "Simple_renderer.h"
#include <Migine/Resource_manager.h>
#include <migine/game_objects/components/Transform.h>

using glm::value_ptr;
using glm::vec3;
using glm::quat;

namespace migine {
	Simple_renderer::Simple_renderer(const Shader& shader, const Mesh& mesh, vec3 position, vec3 scale, quat rotation) :
		Has_shader(shader), Has_mesh(mesh) {
	}

	void Simple_renderer::render(const Camera& camera) {
		if (!shader.GetProgramID()) {
			return;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, value_ptr(camera.GetProjectionMatrix()));
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, value_ptr(get_transform().get_model()));

		mesh.Render();
	}

	Simple_renderer::Simple_renderer() :
		Simple_renderer(get_shader<Shader_id::color>(), get_mesh<Mesh_id::box>()) {
	}
}