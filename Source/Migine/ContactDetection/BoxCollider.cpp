#include "BoxCollider.h"

#include <Migine/ContactDetection/SphereCollider.h>

using namespace Migine;

using EngineComponents::Transform;
using glm::vec3;
using glm::quat;

BoxCollider::BoxCollider(EngineComponents::Transform& transform) :
	BaseCollider(transform) {
}

BoxCollider::BoxCollider(glm::vec3 position, glm::vec3 scale, glm::quat rotation) :
	BaseCollider(position, scale, rotation) {
}

Collision BoxCollider::CheckCollision(BaseCollider* other) {
	return other->CheckCollision(this);
}

Collision BoxCollider::CheckCollision(BoxCollider* other) {
	assert(false); // TODO implementeaza
	return Collision();
}

inline Collision BoxCollider::CheckCollision(SphereCollider* other) { // DRY principle
	return other->CheckCollision(this);
}
