#include "Scene_input.h"
#include "Scene_base.h"

#include <include/gl.h>
#include <Core/Window/WindowObject.h>

namespace migine {

	Scene_input::Scene_input(Scene_base& scene) : scene(scene) {
	}

	void Scene_input::OnKeyPress(int key, int mods) {
		//if (key == GLFW_KEY_F3)
		//	scene.draw_ground_plane = !scene.draw_ground_plane;

		//if (key == GLFW_KEY_F5)
		//	scene->ReloadShaders();

		if (key == GLFW_KEY_ESCAPE)
			scene.Exit();
	}
}