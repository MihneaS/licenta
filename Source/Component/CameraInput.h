#pragma once

#include <Core/Window/InputController.h>

namespace EngineComponents
{
	class Camera;
}

class CameraInput : public InputController
{
	public:
		CameraInput(EngineComponents::Camera *camera);
		void on_input_update(float delta_time, int mods) override;
		void on_key_press(int key, int mods) override;
		void on_mouse_move(int mouse_x, int mouse_y, int delta_x, int delta_y) override;
		void on_mouse_btn_press(int mouse_x, int mouse_y, int button, int mods) override;
		void on_mouse_btn_release(int mouse_x, int mouse_y, int button, int mods) override;

	public:
		EngineComponents::Camera *camera;
};

