#pragma once

#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#include <migine/scenes/Scene_base.h>
#include <migine/utils.h>

namespace migine {
	class Scene_01: public migine::Scene_base {
	public:
		Scene_01();
		~Scene_01();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void old_update(float deltaTimeSeconds);
		void FrameEnd() override;

		void render_simple_mesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model_matrix, glm::vec3 color = glm::vec3(1));

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		unsigned int material_shininess;
		float material_kd;
		float material_ks;

		glm::vec3 light_position;
		glm::vec3 light_direction;
	};
}