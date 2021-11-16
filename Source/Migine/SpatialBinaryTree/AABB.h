#pragma once

#include <Core/Engine.h>
#include <Component/Transform/Transform.h>
#include <Component/SimpleScene.h>
#include <Migine/Renderers/WireframeRenderer.h>
#include <Migine/RenderedObject.h>

namespace Migine {
	class GameObject;

	class AABB : public RenderedObject{
	public:
		glm::vec3 minPos;
		glm::vec3 maxPos;
		bool show = false;

		AABB(glm::vec3 minPos, glm::vec3 maxPos, glm::vec3 color = kAABBColor, int lineWidth = kDefaultLineWidth);
		AABB(GameObject *boundedObject, glm::vec3 color = kAABBColor, int lineWidth = kDefaultLineWidth);
		AABB(std::vector<AABB*> children, glm::vec3 color = kAABBColor, int lineWidth = kDefaultLineWidth);
		AABB() = delete;

		void Render(const EngineComponents::Camera* camera) override;
	};
}