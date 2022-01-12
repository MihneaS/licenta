#include "BoxCollider.h"
#include "SphereCollider.h"

#include <numeric>
#include <algorithm>
#include <cmath>

using namespace Migine;

using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::distance;
using std::tuple;
using std::get;
using std::accumulate;
using std::max;


SphereCollider::SphereCollider(Transform* transform, Mesh* mesh) :
	BaseCollider(transform) {
	ComputeCenterAndRadius(mesh);
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

tuple<vec3, vec3> SphereCollider::ProvideAABBParameters() {
	vec3 centerWorld = transform->GetModel() * vec4{center,1};
	vec3 minPos = {centerWorld.x - radius, centerWorld.y - radius, centerWorld.z - radius};
	vec3 maxPos = {centerWorld.x + radius, centerWorld.y + radius, centerWorld.z + radius};
	return {minPos, maxPos};
}

float SphereCollider::GetDiameter() {
	vec3 scale = transform->GetScale();
	assert(scale.x == scale.y && scale.y == scale.z);
	return scale.x;
}

void SphereCollider::ComputeCenterAndRadius(Mesh* mesh) {
	if (auto cachedCornersIter = cache.find(mesh->GetId()); cachedCornersIter != cache.end()) {
		center = get<vec3>(cachedCornersIter->second);
		radius = get<float>(cachedCornersIter->second);
	} else {
		center = accumulate(mesh->positions.begin(), mesh->positions.end(), vec3{0,0,0});
		radius = accumulate(mesh->positions.begin(), mesh->positions.end(), 0.0f, 
			[=](float accumulator, vec3 point) { return max(accumulator, distance(center, point)); });
		cache[mesh->GetId()] = {center, radius};
	}
}
