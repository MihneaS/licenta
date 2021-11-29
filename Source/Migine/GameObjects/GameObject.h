#pragma once

#include <Migine/RenderedObject.h>
#include <Migine/BroadContactDetection/AABB.h>
#include <Migine/BroadContactDetection/BVH.h>

#include <Core/Engine.h>

#include <string>

namespace Migine {

	class GameObject : public RenderedObject {
		friend class BVH;
	private:
		// should only be accesed by BHV, the parent class of Node.
		// memory managed by a BVH
		BVH::Node* bvhNode;
	public:
		glm::vec3 speed = {0,0,0};
		glm::quat angularSpeed = glm::quat();
		std::string name;

	public:
		GameObject(Mesh* mesh, BaseRenderer* renderer,
			glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
		GameObject(Mesh* mesh, BaseRenderer* renderer, const EngineComponents::Transform& transform);

		virtual void Init();
		virtual void Render(const EngineComponents::Camera* camera) override;
		virtual void Update();
		bool Integrate(const float deltaTime);
	};
}