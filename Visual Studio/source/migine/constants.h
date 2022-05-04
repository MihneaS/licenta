#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
	constexpr glm::vec3 k_default_gravity = {0, -3, 0};//{0, -9.81, 0};
	const glm::mat3 k_i3 = glm::mat3();
	constexpr glm::vec3 k_right =   { 1, 0, 0};
	constexpr glm::vec3 k_left =    {-1, 0, 0};
	constexpr glm::vec3 k_up =      { 0, 1, 0};
	constexpr glm::vec3 k_down =    { 0,-1, 0};
	constexpr glm::vec3 k_back =    { 0, 0, 1};
	constexpr glm::vec3 k_forward = { 0, 0,-1};
	constexpr int k_maximum_position_correcting_iterations = 1000;
	constexpr int k_maximum_velocity_correcting_iterations = 1000;
	constexpr float k_angular_move_limit = 0.2f;
	constexpr float k_float_epsilon = 0.0001f;
	constexpr float k_penetration_epsilon = 0.0f; // TODO revert to non zero! it was 0.001
	constexpr float k_velocity_epsilon = 0.0001f;
	constexpr float k_velocity_restitution_limit = 0.25f;
	constexpr float k_default_restitution = 0.9f;

	enum class Axis {
		ox = 0,
		oy = 1,
		oz = 2
	};
}