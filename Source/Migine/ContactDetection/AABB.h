#pragma once

#include <ostream>

#include <Core/Engine.h>
#include <Migine/Transform.h>
#include <Component/SimpleScene.h>

#include <Migine/Renderers/WireframeRenderer.h>
#include <Migine/RenderedObject.h>

namespace Migine {
	class BaseCollider;

	class AABB {
	public:
		glm::vec3 minPos;
		glm::vec3 maxPos;

		AABB(glm::vec3 minPos, glm::vec3 maxPos);
		AABB(BaseCollider *collider);
		AABB(Mesh* mesh, Transform* transform);
		AABB(const AABB* child0, const AABB* child1);
		AABB() = delete;

		float GetVolume() const;
		glm::vec3 GetCenter() const;
		void EnlargeBy(const AABB* toEnlargeBy);
		void Resize(const AABB* child0, const AABB* child1);
		bool DoesIntersect(const AABB* other) const;
		bool Contains(glm::vec3 point) const;
		//static std::tuple<glm::vec3, glm::vec3> obtainMinPosMaxPos(Mesh* mesh, Transform* transform);
	};
}