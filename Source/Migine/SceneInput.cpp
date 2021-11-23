#include "SceneInput.h"
#include <iostream>

#include <include/gl.h>
#include <Core/Window/WindowObject.h>

#include <Migine/BaseScene.h>
#include <Migine/SceneInput.h>

#include <Component/Transform/Transform.h>

using namespace Migine;

Migine::SceneInput::SceneInput(BaseScene *scene)
{
	this->scene = scene;
}

void Migine::SceneInput::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F3)
		scene->drawGroundPlane = !scene->drawGroundPlane;

	if (key == GLFW_KEY_F5)
		scene->ReloadShaders();

	if (key == GLFW_KEY_ESCAPE)
		scene->Exit();
}