#pragma once

class Mesh;
class Shader;
class InputController;

namespace EngineComponents
{
	class Camera;
	class Transform;
}

#include <Migine/GameObjects/GameObject.h>
#include <Migine/BroadContactDetection/BVH.h>

#include <Core/World.h>

#include <string>
#include <vector>

namespace Migine {
	class BaseScene : public World
	{
		friend class SceneInput;

	public:
		glm::vec3 lightPosition;
		glm::vec3 lightDirection;

	public:
		// memory managed by this
		EngineComponents::Camera* camera;
	private:
		// memory managed by this
		InputController* cameraInput;

		bool drawGroundPlane;
		// memory managed by this
		Mesh* xozPlane;
		// memory managed by this
		Mesh* simpleLine;
		// memory managed by this
		EngineComponents::Transform* objectModel;
	protected:
		BVH bvh;
		//memory managed by this
		std::vector<Migine::GameObject*> gameObjects;



	public:
		BaseScene();
		~BaseScene();

		void RegisterGameObject(GameObject* gameObject);

	protected:
		virtual void DrawCoordinatSystem();
		virtual void DrawCoordinatSystem(const glm::mat4& viewMatrix, const glm::mat4& projectionMaxtix);

		virtual void RenderMesh(Mesh* mesh, Shader* shader, glm::vec3 position, glm::vec3 scale = glm::vec3(1));
		virtual void RenderMesh(Mesh* mesh, glm::vec3 position, glm::vec3 scale = glm::vec3(1));

		virtual void RenderMesh2D(Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix);
		virtual void RenderMesh2D(Mesh* mesh, const glm::mat3& modelMatrix, const glm::vec3& color) const;

		virtual void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

		virtual void ReloadShaders() const final;
		virtual EngineComponents::Camera* GetSceneCamera() const final;
		virtual InputController* GetCameraInput() const final;

	private:
		void InitResources();
	};
}