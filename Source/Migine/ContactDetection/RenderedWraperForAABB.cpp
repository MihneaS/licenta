#include "RenderedWraperForAABB.h"

#include <Migine/ResourceManager.h>
#include <Core/GPU/Mesh.h>
#include <Core/GPU/Shader.h>

using namespace Migine;
using glm::vec3;

RenderedWraperForAABB::RenderedWraperForAABB(AABB* aabb, vec3 color, int lineWidth) :
	RenderedObject(GetMesh<MeshId::boxWireframe>(), 
			new WireframeRenderer(GetShader<ShaderId::color>(), this, color, lineWidth),
			(aabb->minPos + aabb->maxPos)/2.0f, aabb->maxPos - aabb->minPos),
	aabb(aabb) {
}

void RenderedWraperForAABB::Render(const EngineComponents::Camera* camera) {
	transform.SetWorldPosition((aabb->minPos + aabb->maxPos) / 2.0f);
	transform.SetScale(aabb->maxPos - aabb->minPos);
	RenderedObject::Render(camera);
}
