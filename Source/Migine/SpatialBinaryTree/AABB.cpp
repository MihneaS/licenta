#include "AABB.h"
#include <Migine/define.h>
#include <Migine/Renderers/WireframeRenderer.h>
#include <Migine/GameObjects/GameObject.h>
#include <Migine/ResourceManager.h>
#include <vector>
#include <algorithm>

using namespace Migine;
using EngineComponents::Camera;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;
using std::swap;
using std::vector;
using std::min;
using std::max;

AABB::AABB(vec3 minPos, vec3 maxPos, vec3 color, int lineWidth) :
	RenderedObject(GetMesh<MeshId::boxWireframe>(),
			new WireframeRenderer(GetShader<ShaderId::color>(), this, color, lineWidth),
			vec3((minPos+maxPos)/2.0f), vec3(maxPos-minPos), quat()) {
}

AABB::AABB(GameObject* boundedObject, vec3 color, int lineWidth) :
	RenderedObject(GetMesh<MeshId::boxWireframe>(),
			new WireframeRenderer(GetShader<ShaderId::color>(), this, color, lineWidth)) {

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

	transform.SetWorldPosition((minPos + maxPos) / 2.0f);
	transform.SetWorldRotationAndScale(quat(), maxPos - minPos);
}

AABB::AABB(vector<AABB*> children, vec3 color, int lineWidth) :
	RenderedObject(GetResourceManager()->GetMesh(MeshId::boxWireframe),
		new WireframeRenderer(GetResourceManager()->GetShader(ShaderId::color), this, color, lineWidth)) {
	minPos = children[0]->minPos;
	maxPos = children[0]->maxPos;
	for (int i = 1; i < children.size(); i++) {
		minPos.x = min(minPos.x, children[i]->minPos.x);
		maxPos.x = max(maxPos.x, children[i]->maxPos.x);
		minPos.y = min(minPos.y, children[i]->minPos.y);
		maxPos.y = max(maxPos.y, children[i]->maxPos.y);
		minPos.z = min(minPos.z, children[i]->minPos.z);
		maxPos.z = max(maxPos.z, children[i]->maxPos.z);
	}
}

void AABB::Render(const Camera* camera) {
	if (show) {
		RenderedObject::Render(camera);
	}
}
