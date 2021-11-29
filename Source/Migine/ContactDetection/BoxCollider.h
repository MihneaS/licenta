#pragma once

#include <Migine/ContactDetection/BaseCollider.h>

namespace Migine {
	class BoxCollider: public BaseCollider {
	public:
		BoxCollider(EngineComponents::Transform& transform);
		BoxCollider(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());

		Collision CheckCollision(BaseCollider* other) override; // double dispatch T.T I hate it
		Collision CheckCollision(BoxCollider* other) override;
		inline Collision CheckCollision(SphereCollider* other) override;
	};
}