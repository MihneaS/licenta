#include "Laborator3.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;

Laborator3::Laborator3()
{
}

Laborator3::~Laborator3()
{
}

void Laborator3::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	// compute coordinates of square center
	float cx = corner.x + squareSide / 2;
	float cy = corner.y + squareSide / 2;
	
	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;
	
	// initialize angularStep
	angularStep = 0;
	

	Mesh* square1 = Object2D::CreateSquare("square1", corner, squareSide, squareSide, glm::vec3(1, 0, 0), true);
	AddMeshToList(square1);
	
	Mesh* square2 = Object2D::CreateSquare("square2", corner, squareSide, squareSide, glm::vec3(0, 1, 0));
	AddMeshToList(square2);

	Mesh* square3 = Object2D::CreateSquare("square3", corner, squareSide, squareSide, glm::vec3(0, 0, 1));
	AddMeshToList(square3);

	Mesh* square_rot = Object2D::CreateSquare("square_rot", corner, squareSide, squareSide, glm::vec3(1, 0, 1));
	AddMeshToList(square_rot);

	ang_pos = 0;
	ang_pos2 = 0;
	target = 150;
	target_pas = 150;
}

void Laborator3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator3::Update(float deltaTimeSeconds)
{
	// TODO: update steps for translation, rotation, scale, in order to create animations

	float travel_distance = 150;
	float max_scale = 2;
	float ang_speed = 1.5;

	modelMatrix = glm::mat3(1);
	ang_pos += ang_speed*deltaTimeSeconds;
	
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(150, 250 + travel_distance * sin(ang_pos));
	// TODO: create animations by multiplying current transform matrix with matrices from Transform 2D

	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(400, 250);
	modelMatrix *= Transform2D::Translate(50, 50);
	modelMatrix *= Transform2D::Scale(max_scale * cos(ang_pos), max_scale * cos(ang_pos));
	modelMatrix *= Transform2D::Translate(-50, -50);
	//TODO create animations by multiplying current transform matrix with matrices from Transform 2D
	
	RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(650, 250);
	modelMatrix *= Transform2D::Translate(50, 50);
	modelMatrix *= Transform2D::Rotate(cos(ang_pos));
	modelMatrix *= Transform2D::Translate(-50, -50);

	//TODO create animations by multiplying current transform matrix with matrices from Transform 2D
	RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);


	float speed2 = 1;
	modelMatrix = glm::mat3(1);
	ang_pos2 += speed2 * deltaTimeSeconds;
	float pi = 3.1413;
	if (ang_pos2 > pi) {
		ang_pos2 = 0;
		target += 2*target_pas;
	}

	modelMatrix *= Transform2D::Translate(50 + target + target_pas, 50);
	modelMatrix *= Transform2D::Rotate(-ang_pos2);
	modelMatrix *= Transform2D::Translate(-50 - target_pas, -50);


	RenderMesh2D(meshes["square_rot"], shaders["VertexColor"], modelMatrix);
}

void Laborator3::FrameEnd()
{

}

void Laborator3::OnInputUpdate(float deltaTime, int mods)
{
	
}

void Laborator3::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Laborator3::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator3::OnWindowResize(int width, int height)
{
}
