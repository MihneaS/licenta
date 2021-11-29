#pragma once

#include <Component/Transform/Transform.h>
#include <Migine/ContactDetection/Collision.h>

namespace Migine {
	class BoxCollider;
	class SphereCollider;

	class BaseCollider {
		EngineComponents::Transform transform;
	public:
		BaseCollider(EngineComponents::Transform& transform);
		BaseCollider(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		
		virtual Collision CheckCollision(BaseCollider* other) = 0; // double dispatch T.T I hate it
		virtual Collision CheckCollision(BoxCollider* other) = 0;
		virtual Collision CheckCollision(SphereCollider* other) = 0;
	};
}