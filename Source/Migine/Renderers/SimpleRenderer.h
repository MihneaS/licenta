#pragma once

#include <Core/Engine.h>
#include <Migine/Renderers/BaseRenderer.h>
#include <Migine/Camera.h>

namespace Migine {
	class SimpleRenderer : public BaseRenderer {
	public:
		SimpleRenderer(const Shader* shader, RenderedObject* renderedObject);

		virtual void Render(const Camera* camera) override;

	private:
		GLint loc_model_matrix;
		int loc_view_matrix;
		int loc_projection_matrix;

	};

}