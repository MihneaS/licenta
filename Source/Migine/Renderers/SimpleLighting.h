#pragma once

#include <Core/Engine.h>
#include <Migine/Renderers/BaseRenderer.h>
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>

namespace Migine {
	class SimpleLighting : public BaseRenderer {
	private:
		int loc_light_position;
		int loc_light_direction;
		int loc_eye_position;
		int loc_material_shininess;
		int loc_material_kd;
		int loc_material_ks;
		int loc_object_color;
		GLint loc_model_matrix;
		int loc_view_matrix;
		int loc_projection_matrix;

		glm::vec3* lightPosition;
		glm::vec3* lightDirection;
		int materialShininess;
		float materialKd;
		float materialKs;
		glm::vec3 color;

	public:
		SimpleLighting(const Shader* shader, RenderedObject* renderedObject,
			glm::vec3* lightPosition, glm::vec3* lightDirection, int materialShininess, float materialKd, float materialKs,
			glm::vec3 color);

		virtual void Render(const EngineComponents::Camera* camera) override;
	};

}