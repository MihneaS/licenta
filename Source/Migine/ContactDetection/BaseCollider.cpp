#include "BaseCollider.h"
#include <Migine/Transform.h>

using glm::vec3;
using glm::quat;

using namespace Migine;

BaseCollider::BaseCollider(Transform* transform) :
	transform(transform) {
}