#pragma once

#include <Core/Window/InputController.h>

namespace Migine {
	class BaseScene;

	class SceneInput : public InputController
	{
	public:
		SceneInput(BaseScene* camera);
		void OnKeyPress(int key, int mods) override;

	public:
		BaseScene* scene;
	};
}