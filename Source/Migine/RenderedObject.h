#pragma once

#include <Core/Engine.h>
#include <Migine/Transform.h>
#include <Migine/Camera.h>

#include <Migine/Renderers/BaseRenderer.h>

namespace Migine {
	class RenderedObject {
	public:

		Transform transform;
		// memory managed by Resource Manager
		Mesh* mesh;
		// memory managed by Resource Manager
		BaseRenderer* renderer;

		RenderedObject(Mesh* mesh, BaseRenderer* renderer,
			glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		RenderedObject(Mesh* mesh, BaseRenderer* renderer, const Transform& transform);

		~RenderedObject();

		virtual void Render(const Camera* camera);
	};
}