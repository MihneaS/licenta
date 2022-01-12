#include "BoxCollider.h"

#include <Migine/ContactDetection/SphereCollider.h>
#include <Migine/Transform.h>
#include <Migine/utils.h>

#include <algorithm>
#include <vector>
#include <numeric>

using namespace Migine;

using glm::vec3;
using glm::vec4;
using glm::quat;
using std::max;
using std::min;
using std::tuple;
using std::pair;
using std::vector;
using std::accumulate;

BoxCollider::BoxCollider(Transform* transform, Mesh* mesh) :
	BaseCollider(transform) {
	ConstructLocalCorners(mesh);
}

Collision BoxCollider::CheckCollision(BaseCollider* other) {
	return other->CheckCollision(this);
}

Collision BoxCollider::CheckCollision(BoxCollider* other) {
	assert(false); // TODO implementeaza
	return Collision();
}

Collision BoxCollider::CheckCollision(SphereCollider* other) { // DRY principle
	return other->CheckCollision(this);
}

tuple<vec3, vec3> BoxCollider::ProvideAABBParameters() {
	auto& toWorldCoords = transform->GetModel();
	vec3 minPos, maxPos;
	minPos = maxPos = toWorldCoords * vec4(localCorners[0], 1);
	for (int i = 0; i < localCorners.size(); i++) {
		vec3 worldCorner = toWorldCoords * vec4(localCorners[i], 1);
		minPos.x = min(minPos.x, worldCorner.x);
		minPos.y = min(minPos.y, worldCorner.y);
		minPos.z = min(minPos.z, worldCorner.z);
		maxPos.x = max(maxPos.x, worldCorner.x);
		maxPos.y = max(maxPos.y, worldCorner.y);
		maxPos.z = max(maxPos.z, worldCorner.z);
	}
	return {minPos, maxPos};
}

void BoxCollider::ConstructLocalCorners(Mesh* mesh) {
	if (auto cachedCornersIter = cache.find(mesh->GetId()); cachedCornersIter != cache.end()) {
		localCorners = cachedCornersIter->second;
	} else {
		vec3 minPos, maxPos;
		minPos = maxPos = mesh->positions[0];
		for (int i = 1; i < mesh->positions.size(); i++) {
			minPos.x = min(minPos.x, mesh->positions[i].x);
			maxPos.x = max(maxPos.x, mesh->positions[i].x);
			minPos.y = min(minPos.y, mesh->positions[i].y);
			maxPos.y = max(maxPos.y, mesh->positions[i].y);
			minPos.z = min(minPos.z, mesh->positions[i].z);
			maxPos.z = max(maxPos.z, mesh->positions[i].z);
		}
		vec3 center = (minPos + maxPos) / 2.0f;
		float halfSideLength = max(max(maxPos.x - minPos.x, maxPos.y - minPos.y), maxPos.z - minPos.z) / 2.0f;
		localCorners.reserve(8);
		localCorners = {
			center + vec3({ -halfSideLength, -halfSideLength, -halfSideLength }),
			center + vec3({ +halfSideLength, -halfSideLength, -halfSideLength }),
			center + vec3({ -halfSideLength, +halfSideLength, -halfSideLength }),
			center + vec3({ +halfSideLength, +halfSideLength, -halfSideLength }),
			center + vec3({ -halfSideLength, -halfSideLength, +halfSideLength }),
			center + vec3({ +halfSideLength, -halfSideLength, +halfSideLength }),
			center + vec3({ -halfSideLength, +halfSideLength, +halfSideLength }),
			center + vec3({ +halfSideLength, +halfSideLength, +halfSideLength })
		};
		cache[mesh->GetId()] = localCorners;
	}
}
