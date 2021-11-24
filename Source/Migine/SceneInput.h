#pragma once

#include <Core/Window/InputController.h>

namespace Migine {
	class BaseScene;

	class SceneInput : public InputController
	{
	public:
		// memory managed by someone else
		BaseScene* scene;

		SceneInput(BaseScene* camera);

		void OnKeyPress(int key, int mods) override;
	};
}