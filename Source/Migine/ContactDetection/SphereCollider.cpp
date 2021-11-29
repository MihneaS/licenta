#include "BoxCollider.h"
#include "SphereCollider.h"

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

Collision SphereCollider::CheckCollision(BaseCollider* other) {
	return other->CheckCollision(this);
}

Collision SphereCollider::CheckCollision(BoxCollider* other) {
	assert(false); // TODO implementeaza
	return Collision();
}

Collision SphereCollider::CheckCollision(SphereCollider* other) {
	assert(false); // TODO implementeaza
	return Collision();
}
