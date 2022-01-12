#pragma once

#include <Migine/RenderedObject.h>
#include <Migine/ContactDetection/BaseCollider.h>
#include <Migine/GameObjects/RigidBody.h>
#include <Migine/define.h>

#include <Core/Engine.h>

#include <string>

namespace Migine {

	class GameObject : public RenderedObject {
	public:
		glm::vec3 speed = {0,0,0};
		glm::quat angularSpeed = glm::quat();
#ifdef DEBUGGING
		std::string name;
#endif // DEBUGGING
		// memory managed by this
		BaseCollider* collider;
		RigidBody* rigidBody;

	public:
		GameObject(Mesh* mesh, BaseRenderer* renderer, BaseCollider* collider,
			glm::vec3 position = {0,0,0}, glm::vec3 scale = {1,1,1}, glm::quat rotation = glm::quat());
		GameObject(Mesh* mesh, BaseRenderer* renderer, BaseCollider* collider, const EngineComponents::Transform& transform);
		~GameObject();

		virtual void Init();
		virtual void Render(const Camera* camera) override;
		virtual void Update();
		bool Integrate(const float deltaTime);
	};
}