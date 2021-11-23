#include "RenderedObject.h"

using namespace Migine;
using EngineComponents::Transform;
using EngineComponents::Camera;
using glm::vec3;
using glm::quat;


RenderedObject::RenderedObject(Mesh* mesh, BaseRenderer* renderer, const vec3 position, const vec3 scale, const quat rotation) :
	mesh(mesh), renderer(renderer) {
	transform.SetWorldPosition(position);
	transform.SetWorldRotationAndScale(rotation, scale);
}
RenderedObject::RenderedObject(Mesh* mesh, BaseRenderer* renderer, const EngineComponents::Transform& transform) :
	mesh(mesh), transform(transform), renderer(renderer) {
}

RenderedObject::~RenderedObject() {
	if (renderer) {
		delete renderer;
	}
}

void RenderedObject::Render(const Camera* camera) {
	renderer->Render(camera);
}
