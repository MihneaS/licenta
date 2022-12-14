#include <ctime>
#include <iostream>

using namespace std;

#include <Core/Engine.h>

#include <migine/scenes/current_scene.h>

extern "C" {
	// use dedicated graphics card if available
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	// Create a window property structure
	WindowProperties wp;
	wp.resolution = glm::ivec2(1280, 720);

	// Init the Engine and create a new window with the defined properties
	WindowObject* window = Engine::Init(wp);

	// Create a new 3D world and start running it
	World& world = migine::get_current_scene();
	world.init();
	world.run();

	// Signals to the Engine to release the OpenGL context
	Engine::Exit();

	return 0;
}