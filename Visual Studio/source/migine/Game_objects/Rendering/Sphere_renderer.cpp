#include "Sphere_renderer.h"
#include <Migine/Resource_manager.h>
#include <migine/game_objects/components/Transform.h>
#include <migine/game_objects/shapes/Sphere.h>

using glm::value_ptr;
using glm::vec3;
using glm::quat;

namespace migine {
	Sphere_renderer::Sphere_renderer(const Shader& shader, const Mesh& mesh, const Sphere& sphere, vec3 position, vec3 scale, quat rotation) :
		Has_shader(shader), Has_mesh(mesh), sphere(sphere) {
	}

	void Sphere_renderer::render(const Camera& camera) {
		if (!shader.GetProgramID()) {
			return;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader.Use();
		glUniformMatrix4fv(shader.loc_view_matrix, 1, GL_FALSE, value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(shader.loc_projection_matrix, 1, GL_FALSE, value_ptr(camera.GetProjectionMatrix()));
		glUniformMatrix4fv(shader.loc_model_matrix, 1, GL_FALSE, value_ptr(get_transform().get_model()));
		glUniform1fv(shader.loc_sphere_center, 1, value_ptr(sphere.get_center_world()));
		glUniform1f(shader.loc_sphere_center, sphere.get_radius());
		mesh.Render();
	}

	Sphere_renderer::Sphere_renderer(const Sphere& sphere) :
		Sphere_renderer(get_shader<Shader_id::sphere>(), get_mesh<Mesh_id::box>(), sphere) {
	}
}