#include "SimpleLighting.h"
/*
#include <Core/Engine.h>
#include <Migine/RenderedObject.h>

using namespace Migine;
using glm::value_ptr;
using glm::vec3;



SimpleLighting::SimpleLighting(const Shader* shader, RenderedObject* renderedObject,
	vec3* lightPosition, vec3* lightDirection, int materialShiness, float materialKd, float materialKs, vec3 color) :
	BaseRenderer(shader, renderedObject), lightPosition(lightPosition), lightDirection(lightDirection),
	materialShininess(materialShininess), materialKd(materialKd), materialKs(materialKs), color(color) {
	loc_light_position = glGetUniformLocation(shader->program, "light_position");
	loc_light_direction = glGetUniformLocation(shader->program, "light_direction");
	loc_material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	loc_material_kd = glGetUniformLocation(shader->program, "material_kd");
	loc_material_ks = glGetUniformLocation(shader->program, "material_ks");
	loc_object_color = glGetUniformLocation(shader->program, "object_color");
	loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	loc_view_matrix = glGetUniformLocation(shader->program, "View");
	loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
}

void SimpleLighting::Render(const EngineComponents::Camera* camera) {
	if (!renderedObject->mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	glUniform3fv(loc_light_position, 1, value_ptr(*lightPosition));
	glUniform3fv(loc_light_direction, 1, value_ptr(*lightDirection));
	glUniform3fv(loc_eye_position, 1, value_ptr(camera->transform->GetWorldPosition()));
	glUniform1i(loc_material_shininess, materialShininess);
	glUniform1f(loc_material_kd, materialKd);
	glUniform1f(loc_material_ks, materialKs);
	glUniform3fv(loc_object_color, 1, value_ptr(color));
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, value_ptr(renderedObject->transform.GetModel()));
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
	glBindVertexArray(renderedObject->mesh->GetBuffers()->VAO);
	glDrawElements(renderedObject->mesh->GetDrawMode(), static_cast<int>(renderedObject->mesh->indices.size()), GL_UNSIGNED_INT, 0);
	//shader->Use();
	//glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
	//glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
	//glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, value_ptr(transform->GetModel()));

	//mesh->Render();
}
*/






