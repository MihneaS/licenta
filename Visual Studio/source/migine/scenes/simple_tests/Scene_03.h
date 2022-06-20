#pragma once

#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#include <migine/scenes/Scene_base.h>
#include <migine/utils.h>

namespace migine {
	/*
	 * Scene for testing contact resolution betwin sphere and cube
	 */
	class Scene_03: public migine::Scene_base {
	public:
		Scene_03();
		~Scene_03();

		void init() override;

		void frame_start() override;
		void update(float delta_time_seconds) override;
		void frame_end() override;

		void on_input_update(float delta_time, int mods) override;
		void on_key_press(int key, int mods) override;
		void on_key_release(int key, int mods) override;
		void on_mouse_move(int mouse_x, int mouse_y, int delta_x, int delta_y) override;
		void on_mouse_btn_press(int mouse_x, int mouse_y, int button, int mods) override;
		void on_mouse_btn_release(int mouse_x, int mouse_y, int button, int mods) override;
		void on_mouse_scroll(int mouse_x, int mouse_y, int offset_x, int offset_y) override;
		void on_window_resize(int width, int height) override;


		void modify_contacts(std::vector<std::unique_ptr<Contact>>& contacts) override;

		unsigned int material_shininess;
		float material_kd;
		float material_ks;

		glm::vec3 light_position;
		glm::vec3 light_direction;
	};
}