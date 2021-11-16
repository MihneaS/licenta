#pragma once

#include <Migine/RenderedObject.h>
#include <Migine/SpatialBinaryTree/AABB.h>
#include <Core/Engine.h>

namespace Migine {
	class GameObject : public RenderedObject {
	public:
		AABB aabb;

		GameObject(Mesh* mesh, BaseRenderer* renderer,
			glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		GameObject(Mesh* mesh, BaseRenderer* renderer, const EngineComponents::Transform& transform);

		virtual void Init();
		virtual void Render(const EngineComponents::Camera* camera) override;
	};
}