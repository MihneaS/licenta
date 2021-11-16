#pragma once

#include <Core/Engine.h>
#include <Component/Transform/Transform.h>
#include <Component/Camera/Camera.h>
#include <Migine/Renderers/BaseRenderer.h>

namespace Migine {
	class RenderedObject {
	public:

		EngineComponents::Transform transform;
		Mesh* mesh;
		BaseRenderer* renderer;

		RenderedObject(Mesh* mesh, BaseRenderer* renderer,
			glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		RenderedObject(Mesh* mesh, BaseRenderer* renderer, const EngineComponents::Transform& transform);

		~RenderedObject();

		virtual void Render(const EngineComponents::Camera* camera);
	};
}