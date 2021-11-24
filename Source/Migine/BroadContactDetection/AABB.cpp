#include "AABB.h"
#include <Migine/define.h>
#include <Migine/Renderers/WireframeRenderer.h>
#include <Migine/GameObjects/GameObject.h>
#include <Migine/ResourceManager.h>
#include <vector>
#include <algorithm>

using namespace Migine;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::vector;
using std::min;
using std::max;

AABB::AABB(vec3 minPos, vec3 maxPos) :
	minPos(minPos), maxPos(maxPos) {
}

AABB::AABB(GameObject* boundedObject) {
	vector<vec4> points;
	points.reserve(boundedObject->mesh->positions.size());
	for (auto p : boundedObject->mesh->positions) {
		points.emplace_back(boundedObject->transform.GetModel() * vec4(p.x, p.y, p.z, 1));
	}
	minPos = maxPos = points[0];
	for (int i = 1; i < points.size(); i++) {
		minPos.x = min(minPos.x, points[i].x);
		maxPos.x = max(maxPos.x, points[i].x);
		minPos.y = min(minPos.y, points[i].y);
		maxPos.y = max(maxPos.y, points[i].y);
		minPos.z = min(minPos.z, points[i].z);
		maxPos.z = max(maxPos.z, points[i].z);
	}
}

AABB::AABB(const AABB* child0, const AABB* child1) {
	Resize(child0, child1);
}

float AABB::GetVolume() const {
	vec3 tmp = maxPos - minPos;
	return tmp.x * tmp.y * tmp.z;
}

glm::vec3 Migine::AABB::GetCenter() const
{
	return (minPos+maxPos)/2.0f;
}

void AABB::EnlargeBy(const AABB* toEnlargeBy) {
	Resize(this, toEnlargeBy);
}

void AABB::Resize(const AABB* child0, const AABB* child1) {
	minPos.x = min(child0->minPos.x, child1->minPos.x);
	maxPos.x = max(child0->maxPos.x, child1->maxPos.x);
	minPos.y = min(child0->minPos.y, child1->minPos.y);
	maxPos.y = max(child0->maxPos.y, child1->maxPos.y);
	minPos.z = min(child0->minPos.z, child1->minPos.z);
	maxPos.z = max(child0->maxPos.z, child1->maxPos.z);
}
