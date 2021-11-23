#pragma once

#include <Component/Camera/Camera.h>

#include <Migine/RenderedObject.h>
#include <Migine/BroadContactDetection/AABB.h>
#include <Migine/constants.h>

namespace Migine {
	class RenderedWraperForAABB : public RenderedObject {
		AABB* aabb;
	public:
		RenderedWraperForAABB(AABB* aabb, glm::vec3 color = kAABBColor, int lineWidth = kDefaultLineWidth);
		
		void Render(const EngineComponents::Camera* camera) override;
	};
}