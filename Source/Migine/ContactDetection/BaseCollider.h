#pragma once

#include <Migine/Transform.h>
#include <Migine/ContactDetection/Collision.h>
#include <Migine/ContactDetection/BVH.h>
#include <Migine/define.h>

#include <tuple>
#include <string>

namespace Migine {
	class BoxCollider;
	class SphereCollider;
	//class GameObject;

	class BaseCollider {
		//friend class GameObject;
		friend class BVH;

	protected:
		// memory managed by someone else
		Transform* transform;
		// memory managed by bvh
		BVH::Node* bvhNode = nullptr;
#ifdef DEBUGGING
	public:
		std::string* name;
#endif // DEBUGGING

	public:
		BaseCollider(Transform* transform);
		
		virtual Collision CheckCollision(BaseCollider* other) = 0; // double dispatch T.T I hate it
		virtual Collision CheckCollision(BoxCollider* other) = 0;
		virtual Collision CheckCollision(SphereCollider* other) = 0;

		virtual std::tuple<glm::vec3, glm::vec3> ProvideAABBParameters() = 0;
	};
}