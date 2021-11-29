#include "BaseCollider.h"

using EngineComponents::Transform;

using glm::vec3;
using glm::quat;

using namespace Migine;

BaseCollider::BaseCollider(Transform& transform) :
	transform(transform) {
}

BaseCollider::BaseCollider(vec3 position, vec3 scale, quat rotation) {
	transform.SetWorldPosition(position);
	transform.SetWorldRotationAndScale(rotation, scale);
}