#pragma once

#include <Core/Engine.h>

namespace migine {
	constexpr glm::vec3 k_default_color = {1,0,1};
	constexpr float k_default_line_width = 5;
	constexpr glm::vec3 k_aabb_color = {0.1,0.6,0.1};
	constexpr float k_deg_to_rad = glm::pi<float>() / 180.0f;
	constexpr int k_box_material_shiness = 30;
	constexpr float k_box_material_kd = 0.5;
	constexpr float k_box_material_ks = 0.5;
	constexpr glm::vec3 k_box_color = {0.0f / 256.0f, 153.0f / 256.0f, 255.0f};
	constexpr glm::vec3 k_vec3_zero = {0,0,0};
	constexpr int k_no_index = -1;
	constexpr glm::vec3 k_default_gravity = {0, -9.81, 0};
}