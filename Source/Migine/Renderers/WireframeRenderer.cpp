#include "WireframeRenderer.h"
#include <Migine/Transform.h>
#include <Migine/RenderedObject.h>

using namespace Migine;
using glm::vec3;
using glm::quat;

WireframeRenderer::WireframeRenderer(Shader* shader, RenderedObject* renderedObject, vec3 color, float lineWidth) :
	BaseRenderer(shader, renderedObject), color(color), lineWidth(lineWidth) {
}

void WireframeRenderer::Render(const Camera* camera) {
	glLineWidth(lineWidth);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(renderedObject->transform.GetModel()));
	glUniform3fv(shader->GetUniformLocation("color"), 1, glm::value_ptr(color));
	renderedObject->mesh->Render();
}
