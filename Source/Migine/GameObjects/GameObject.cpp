#include "GameObject.h"
#include <Migine/BaseScene.h>
#include <Migine/constants.h>
#include <sstream>

using glm::vec3;
using glm::quat;
using EngineComponents::Transform;
using EngineComponents::Camera;
using namespace Migine;
using std::stringstream;

static int gameObjectCounter = 0;

Migine::GameObject::GameObject(Mesh* mesh, BaseRenderer* renderer,
		const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation) :
	RenderedObject(mesh, renderer, position, scale, rotation) {
	stringstream ss;
	if (mesh) {
		ss << meshIdToIdNames[mesh->GetId()] << " ";
	} else {
		ss << "NoMesh ";
	}
	ss << gameObjectCounter++;
	name = ss.str();
}

Migine::GameObject::GameObject(Mesh* mesh, BaseRenderer* renderer, const EngineComponents::Transform& transform) :
	RenderedObject(mesh, renderer, transform) {
	stringstream ss;
	if (mesh) {
		ss << meshIdToIdNames[mesh->GetId()] << " ";
	} else {
		ss << "NoMesh ";
	}
	ss << gameObjectCounter++;
	name = ss.str();
}

void GameObject::Init() {
}

void GameObject::Render(const Camera* camera) {
	RenderedObject::Render(camera);
}

void GameObject::Update() {
}

bool GameObject::Integrate(const float deltaTime) {
	bool hasMoved;
	if (speed == kVec3Zero && angularSpeed == quat()) {
		hasMoved = false;
	} else {
		hasMoved = true;
	}
	vec3 position = transform.GetWorldPosition();
	position += speed * deltaTime;
	transform.SetWorldPosition(position);
	quat rotation = transform.GetWorldRotation();
	rotation *= angularSpeed;
	transform.SetWorldRotation(rotation);
	return hasMoved;
}
