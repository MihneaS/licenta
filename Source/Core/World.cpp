#include "World.h"

#include <Core/Engine.h>
#include <Component/CameraInput.h>
#include <Component/Transform/Transform.h>

World::World()
{
	previous_time = 0;
	elapsed_time = 0;
	delta_time = 0;
	paused = false;
	should_close = false;

	window = Engine::GetWindow();
}

void World::run()
{
	if (!window)
		return;

	while (!window->ShouldClose())
	{
		loop_update();
	}
}

void World::pause()
{
	paused = !paused;
}

void World::exit()
{
	should_close = true;
	window->Close();
}

double World::get_last_frame_time()
{
	return delta_time;
}

void World::compute_frame_delta_time()
{
	elapsed_time = Engine::GetElapsedTime();
	delta_time = elapsed_time - previous_time;
	previous_time = elapsed_time;
}

void World::loop_update()
{
	// Polls and buffers the events
	window->PollEvents();

	// Computes frame deltaTime in seconds
	compute_frame_delta_time();

	// Calls the methods of the instance of InputController in the following order
	// OnWindowResize, OnMouseMove, OnMouseBtnPress, OnMouseBtnRelease, OnMouseScroll, OnKeyPress, OnMouseScroll, OnInputUpdate
	// OnInputUpdate will be called each frame, the other functions are called only if an event is registered
	window->UpdateObservers();

	// Frame processing
	frame_start();
	update(static_cast<float>(delta_time));
	frame_end();

	// Swap front and back buffers - image will be displayed to the screen
	window->SwapBuffers();
}