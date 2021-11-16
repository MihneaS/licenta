#include "SimpleRenderer.h"

#include <Migine/RenderedObject.h>
#include <Core/Engine.h>

using namespace Migine;
using glm::value_ptr;
using glm::vec3;



SimpleRenderer::SimpleRenderer(const Shader* shader, RenderedObject* renderedObject) :
	BaseRenderer(shader, renderedObject) {
	loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	loc_view_matrix = glGetUniformLocation(shader->program, "View");
	loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
}

void SimpleRenderer::Render(const EngineComponents::Camera* camera) {
	if (!renderedObject->mesh || !shader || !shader->GetProgramID())
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, value_ptr(renderedObject->transform.GetModel()));

	renderedObject->mesh->Render();
}







