#include "Laborator8.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <Core/Engine.h>

#include <utility>

#include <Migine/GameObjects/Box.h>
#include <Migine/GameObjects/Sphere.h>
#include <Migine/utils.h>
#include <Migine/ResourceManager.h>


using namespace std;
using glm::vec3;
using glm::quat;
using EngineComponents::Transform;
using EngineComponents::Camera;
using namespace Migine;

Migine::GameObject* tmp;
Migine::GameObject* tmp2;


Laborator8::Laborator8() {
}

Laborator8::~Laborator8() {
	for (auto gameObject : gameObjects) {
		delete gameObject;
	}
}

void Laborator8::Init()
{
	ResourceManager* rm = ResourceManager::GetInstance();
	

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 1, 1);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}

	// create test Migine::Box
	{
		RegisterGameObject(new Migine::Box({2, 2, -2}, {1, 2, 1}, EulerAnglesDegToQuat({45, 0, 45})));
	}

	//create test Migine::Sphere
	{
		RegisterGameObject(new Migine::Sphere({-2, 2, -2}));
	}


	{
		RegisterGameObject(new Migine::Sphere({2, 5, 0}));
		RegisterGameObject(new Migine::Sphere({-2, 5, 0}));
		RegisterGameObject(new Migine::Sphere({0, 5, 2}));
		RegisterGameObject(new Migine::Sphere({0, 5, -2}));
		RegisterGameObject(new Migine::Sphere({6, 5, 0}));
	}
	{
		Migine::Sphere* s = new Migine::Sphere({-6, 5, 0});
		RegisterGameObject(s);
		tmp = s;
	}
	{
		auto b = new Migine::Box({0, 8, -2}, {20, 0.2, 20});
		b->name = "Acoperitor";
		RegisterGameObject(b);
		tmp2 = b;
	}

	for (auto gameObject : gameObjects) {
		gameObject->Init();
	}

}

void Laborator8::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator8::Update(float deltaTimeSeconds)
{
	float capedDeltaTime = min(deltaTimeSeconds, 1.0f / 20);
	OldUpdate(capedDeltaTime);
	for (auto gameObject : gameObjects) {
		bool hasMoved = gameObject->Integrate(capedDeltaTime);
		if (hasMoved) {
			bvh.Update(gameObject->collider);
		}
	}
	static float t = 0;
	t += capedDeltaTime;
	tmp->speed = { 0, sin(t), 0 };
	if (tmp2->transform.GetWorldPosition().y >= 7) {
		tmp2->speed = { 0, -0.3, 0 };
	} else if (tmp2->transform.GetWorldPosition().y <= -1) {

		tmp2->speed = { 0, 0.3, 0 };
	}
	for (auto gameObject : gameObjects) {
		gameObject->Render(this->GetSceneCamera());
	}
	bvh.RenderAll(camera);

	static float lastPrintingTime = 0;
	static float totalTime = 0;
	static int totalFrames = 0;
	static int framesSincePrinting = 0;
	framesSincePrinting++;
	totalFrames++;
	totalTime += deltaTimeSeconds;
	if (float deltaTimePrinting = totalTime - lastPrintingTime; deltaTimePrinting > 0.66) {
		stringstream ss;
		ss << "fps:" << framesSincePrinting / deltaTimePrinting << ";";
		framesSincePrinting = 0;
		lastPrintingTime = totalTime;
		ss << " broad contacts:" << bvh.GetContactCount() << ";";
		ss << " insertions:" << bvh.insertionCount << ";";
		ss << " broad intersection checks:" << bvh.AABBIntersectionOperationsCount << ";";
		ss << " time:" << totalTime << ";";
		ss << " frames:" << totalFrames << ";";
		ContinousPrintLineReset();
		ContinousPrint(ss.str());
	}
}

void Laborator8::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator8::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator8::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		// Control light position using on W, A, S, D, E, Q
		if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * speed;
	}
}

void Laborator8::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Laborator8::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator8::OnWindowResize(int width, int height)
{
}

void Laborator8::OldUpdate(float deltaTimeSeconds) {
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		RenderSimpleMesh(GetMesh<MeshId::sphere>(), GetShader<ShaderId::lab8>(), modelMatrix);
		//RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		RenderSimpleMesh(GetMesh<MeshId::box>(), GetShader<ShaderId::lab8>(), modelMatrix);
		//RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 1, 0));
		RenderSimpleMesh(GetMesh<MeshId::box>(), GetShader<ShaderId::lab8>(), modelMatrix, glm::vec3(0, 0.5, 0));
		//RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
	}

	// Render ground
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		RenderSimpleMesh(GetMesh<MeshId::plane>(), GetShader<ShaderId::lab8>(), modelMatrix);
		//RenderMesh(meshes["plane"], shaders["Simple"], modelMatrix);
	}

	// Render the point light in the scene
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, lightPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(GetMesh<MeshId::sphere>(), GetShader<ShaderId::lab8>(), modelMatrix);
	}
}