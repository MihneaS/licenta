#pragma once

#include <Migine/ContactDetection/BaseCollider.h>

namespace Migine {
	class SphereCollider: public BaseCollider {
	public:
		SphereCollider(EngineComponents::Transform& transform);
		SphereCollider(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());

		Collision CheckCollision(BaseCollider* other) override; // double dispatch T.T I hate it
		Collision CheckCollision(BoxCollider* other) override;
		Collision CheckCollision(SphereCollider* other) override;
	};
}