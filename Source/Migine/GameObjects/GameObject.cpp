#include "GameObject.h"
#include <Migine/BaseScene.h>
#include <Migine/constants.h>
#include <Migine/define.h>

#include <sstream>

using glm::vec3;
using glm::quat;
using namespace Migine;
using std::stringstream;

static int gameObjectCounter = 0;

Migine::GameObject::GameObject(Mesh* mesh, BaseRenderer* renderer, BaseCollider* collider,
		const glm::vec3 position, const glm::vec3 scale, const glm::quat rotation) :
	RenderedObject(mesh, renderer, position, scale, rotation), collider(collider) {
#ifdef DEBUGGING
	stringstream ss;
	if (mesh) {
		ss << meshIdToIdNames[mesh->GetId()] << " ";
	} else {
		ss << "NoMesh ";
	}
	ss << gameObjectCounter++;
	name = ss.str();
	this->collider->name = &name;
#endif // DEBUGGING
}

Migine::GameObject::GameObject(Mesh* mesh, BaseRenderer* renderer, BaseCollider* collider, const EngineComponents::Transform& transform) :
	RenderedObject(mesh, renderer, transform), collider(collider) {
#ifdef DEBBUGING
	stringstream ss;
	if (mesh) {
		ss << meshIdToIdNames[mesh->GetId()] << " ";
	} else {
		ss << "NoMesh ";
	}
	ss << gameObjectCounter++;
	name = ss.str();
	this->collider->name = &name;
#endif //DEBUGGING
}

GameObject::~GameObject() {
	if (collider) {
		delete collider;
	}
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
