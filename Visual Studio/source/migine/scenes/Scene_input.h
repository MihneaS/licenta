#pragma once

#include <Core/Window/InputController.h>

namespace migine {
	class Scene_base;

	class Scene_input : public InputController
	{
	public:
		// memory managed by someone else
		Scene_base& scene;

		Scene_input(Scene_base& camera);

		void on_key_press(int key, int mods) override;
	};
}