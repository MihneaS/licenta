#pragma once

#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#include <migine/scenes/Scene_base.h>
#include <migine/utils.h>

namespace migine {
	/*
	 * Scene for testing contact resolution with rotation between 2 cubes
	 */
	class Scene_15 : public migine::Scene_base {
	public:
		Scene_15();
		~Scene_15();

		void init() override;

		void frame_start() override;
		void update(float delta_time_seconds) override;
		void frame_end() override;

		void on_key_press(int key, int mods) override;
		void modify_contacts(std::vector<std::unique_ptr<Contact>>& contacts) override;
	};
}