#pragma once

#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#include <migine/scenes/Scene_base.h>
#include <migine/utils.h>

namespace migine {
	/*
	 * Scene for testing contact resolution with rotation between 2 cubes
	 */
	class Scene_08 : public migine::Scene_base {
	public:
		Scene_08();
		~Scene_08();

		void init() override;

		void frame_start() override;
		void update(float delta_time_seconds) override;
		void frame_end() override;
	};
}