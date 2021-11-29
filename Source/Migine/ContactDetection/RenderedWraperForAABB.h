#pragma once

#include <Component/Camera/Camera.h>

#include <Migine/RenderedObject.h>
#include <Migine/ContactDetection/AABB.h>
#include <Migine/constants.h>

namespace Migine {
	class RenderedWraperForAABB : public RenderedObject {
		// memory managed by someone else
		AABB* aabb;
	public:
		RenderedWraperForAABB(AABB* aabb, glm::vec3 color = kAABBColor, int lineWidth = kDefaultLineWidth);
		
		void Render(const EngineComponents::Camera* camera) override;
	};
}