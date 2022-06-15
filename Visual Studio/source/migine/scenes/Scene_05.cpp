#include "Scene_05.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <Core/Engine.h>

#include <utility>

#include <migine/game_objects/shapes/Box.h>
#include <migine/game_objects/shapes/Sphere.h>
#include <migine/utils.h>
#include <migine/Resource_manager.h>
#include <migine/define.h>

#include <tuple>

using glm::vec3;
using glm::quat;
using glm::mat4;
using glm::mat3;

using std::move;
using std::make_unique;
using std::min;

using std::cout;

namespace migine {

	// inspired by get_closest_points_between_segemnts from https://github.com/godotengine/godot/blob/master/core/math/geometry_3d.h
	std::tuple<vec3, vec3, bool> get_closest_points_between_segments(vec3 seg0_p0, vec3 seg0_p1, vec3 seg1_p0, vec3 seg1_p1){
		// Do the function 'd' as defined by pb. I think it's a dot product of some sort.
//#define d_of(m, n, o, p) ((m.x - n.x) * (o.x - p.x) + (m.y - n.y) * (o.y - p.y) + (m.z - n.z) * (o.z - p.z))
#define d_of(v1, v2, v3, v4) = dot(v1-v2, v3-v4)

		// Calculate the parametric position on the 2 curves, mua and mub.
		//float mua = (d_of(seg0_p0, seg1_p0, seg1_p1, seg1_p0) * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0) - d_of(seg0_p0, seg1_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0)) / (d_of(seg0_p1, seg0_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0) - d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0));
		//float mub = (d_of(seg0_p0, seg1_p0, seg1_p1, seg1_p0) + mua * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0)) / d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0);
#undef d_of

		//float mua = (dot(seg0_p0-seg1_p0, seg1_p1-seg1_p0)*dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0) - dot(seg0_p0-seg1_p0, seg0_p1-seg0_p0)*dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0)) / 
			//(dot(seg0_p1-seg0_p0, seg0_p1-seg0_p0)*dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0) - dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0) * dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0));
		//float mub = (dot(seg0_p0-seg1_p0, seg1_p1-seg1_p0) + mua*dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0)) / dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0);

		// fie seg0_p0 = p0, seg0_p1 = p1; seg1_p0 = q0; seg1_p1 = q1;
		vec3 q0p0 = seg0_p0 - seg1_p0;
		vec3 seg1 = seg1_p1 - seg1_p0; // q0q1
		vec3 seg0 = seg0_p1 - seg0_p0; // p0p1

		//								seg1 || seg0 => seg0 = a*seg1

		//d2*d2/d4*d5 ~= 1

		float s0_dot_s1 = dot(seg1, seg0);
		float s0_dot_s0 = dot(seg0, seg0);
		float s1_dot_s1 = dot(seg1, seg1);

		// if axis are not paralel, do the initial algorithm
		if (!is_equal_aprox(s0_dot_s1 * s0_dot_s1 / (s0_dot_s0 * s1_dot_s1), 1)) {

			float d1 = dot(q0p0, seg1); //	= d1
			float d2 = s0_dot_s1;           //	= a d4
			float d1d2 = d1 * d2;           //	= a d1 d3
			float d3 = dot(q0p0, seg0); //	= a d1
			float d4 = s1_dot_s1;           //	= d4
			float d3d4 = d3 * d4;           //	= a d1 d4
			float dif1 = d1d2 - d3d4;       //	= 0
			float d5 = s0_dot_s0;           //	= a a d4 
			// d6 = d4
			float d5d6 = d5 * d4;           //	= a a d4 d4
			// d7 = d2
			// d8 = d2
			float d7d8 = d2 * d2;           //	= a a d4 d4
			float dif2 = d5d6 - d7d8;       //	= 0
			float mua = dif1 / dif2;
			// d9 = d1
			// d10 = d2
			// d11 = d4
			float add1 = d1 + mua * d2;
			float mub = add1 / d4;
			float mua2 = (dot(seg0_p0 - seg1_p0, seg1_p1 - seg1_p0) * dot(seg1_p1 - seg1_p0, seg0_p1 - seg0_p0) - dot(seg0_p0 - seg1_p0, seg0_p1 - seg0_p0) * dot(seg1_p1 - seg1_p0, seg1_p1 - seg1_p0)) /
				(dot(seg0_p1 - seg0_p0, seg0_p1 - seg0_p0) * dot(seg1_p1 - seg1_p0, seg1_p1 - seg1_p0) - dot(seg1_p1 - seg1_p0, seg0_p1 - seg0_p0) * dot(seg1_p1 - seg1_p0, seg0_p1 - seg0_p0));
			float mub2 = (dot(seg0_p0 - seg1_p0, seg1_p1 - seg1_p0) + mua * dot(seg1_p1 - seg1_p0, seg0_p1 - seg0_p0)) / dot(seg1_p1 - seg1_p0, seg1_p1 - seg1_p0);

			// Clip the value between [0..1] constraining the solution to lie on the original curves.
			bool clipped = false;
			if (mua < 0) {
				mua = 0;
				clipped = true;
			}
			if (mub < 0) {
				mub = 0;
				clipped = true;
			}
			if (mua > 1) {
				mua = 1;
				clipped = true;
			}
			if (mub > 1) {
				mub = 1;
				clipped = true;
			}
			return {lerp(seg0_p0, seg0_p1, mua), lerp(seg1_p0, seg1_p1, mub), clipped};
		} else { // else return projection of mid_point
			vec3 mid_point = (seg0_p0 + seg0_p1 + seg1_p0 + seg1_p1) / 4.0f;
			vec3 res_p0 = project_point_onto_axis(mid_point, seg0_p0, seg0_p1);
			vec3 res_p1 = project_point_onto_axis(mid_point, seg1_p0, seg1_p1);
			bool clipped = res_p0 != mid_point || res_p1 != mid_point;
			return {res_p0, res_p1, clipped};
		}
	}



	void print_mat4(mat4 mat) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << mat[i][j] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	void print_mat3(mat3 mat) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << mat[i][j] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	Scene_05::Scene_05() {
	}

	Scene_05::~Scene_05() {
	}

	void Scene_05::init() {

		{ //defualt forces
			default_fs_gen.clear();
		}

		{
			migine::Transform transform({0.5,0.5,0.5}, {2,2,2}, euler_angles_deg_to_quat({90,0,0}));
			auto& to_world = transform.get_model();
			auto inv = glm::inverse(to_world);
			auto trsp_inv = glm::transpose(inv);
			mat3 some_mat3 = {{2,0,0}, {0,2,0}, {0,0,2}};
			mat4 some_mat4 = mat4(some_mat3);
			print_mat4(some_mat4);
			//mat4 coords_changed = inv * some_mat4 * trsp_inv;
			mat4 coords_changed = trsp_inv * some_mat4 * inv;
			//mat4 coords_changed2 = trsp_inv * some_mat3 * inv;
			print_mat4(coords_changed);
			mat3 m3 = coords_changed;
			print_mat3(m3);

			vec3 v1{1,0,0};
			vec3 v2{0,1,0};
			vec3 v3{0};

			cout << "cross vs skew_mat:" << glm::cross(v1, v2).z << " " << (get_skew_symmetric(v1) * v2).z << "\n";


			vec3 y{0,1,0};
			vec3 x{1,0,0};
			quat qx(0, 1, 0, 0);
			quat qy(0, 0, 1, 0);
			quat q(cos(90 * k_deg_to_rad), vec3{1,0,0});
			cout << "y doar o rotatie: " << q * y << "\n";
			cout << "y ambele rotatii: " << q * y * glm::conjugate(q) << "\n";
			cout << "x doar o rotatie: " << q * x << "\n";
			cout << "x ambele rotatii: " << q * x * glm::conjugate(q) << "\n";
			cout << "qy doar o rotatie: " << q * qy << "\n";
			cout << "qy ambele rotatii: " << q * qy * glm::conjugate(q) << "\n";
			cout << "qx doar o rotatie: " << q * qx << "\n";
			cout << "qx ambele rotatii: " << q * qx * glm::conjugate(q) << "\n";
		}


		do {
			// aici nu ar trebui sa fie coliziune :C
			bool found_new_contact = false;
			auto this_corner1 = vec3{-0.0331051350, 1.74386311, -3.38216639};
			auto this_corner2 = vec3{0.153688371, 1.65321159, -4.36032200};
			auto other_corner1 = vec3{0.122231752, 2.29461193, -4.01483059};
			auto other_corner2 = vec3{-0.526171684, 2.36624670, -3.25691223};

			cout << "this muchie len: " << glm::length(this_corner1 - this_corner2) << "\n";
			cout << "other muchie len: " << glm::length(other_corner1 - other_corner2) << "\n";

			auto this_center = vec3{0.688284874, 2.01041079, -3.78029728};
			auto other_center = vec3{-0.688284874, 1.98959088, -4.01970243};

			auto [pt_on_this_edge, pt_on_other_edge, clipped] = get_closest_points_between_segments(this_corner1, this_corner2, other_corner1, other_corner2);
			static_assert(std::is_same<decltype(clipped), bool>());
			if (clipped) {
				continue;
			}
			assert(is_point_on_axis(this_corner1, this_corner2, pt_on_this_edge));
			assert(is_point_on_axis(other_corner1, other_corner2, pt_on_other_edge));
			//assert(is_point_on_axis2(this_corner1, this_corner2, pt_on_this_edge));
			//assert(is_point_on_axis2(other_corner1, other_corner2, pt_on_other_edge));
			// Distance^2 from Other's Center to This Edge (edge of this)
			float d2_oc_to_te = distance2(other_center, pt_on_this_edge);
			float d2_oc_to_oe = distance2(other_center, pt_on_other_edge);
			if (d2_oc_to_te > d2_oc_to_oe) {
				continue;
			}
			float d2_tc_to_oe = distance2(this_center, pt_on_other_edge);
			float d2_tc_to_te = distance2(this_center, pt_on_this_edge);
			if (d2_tc_to_oe > d2_tc_to_te) {
				continue;
			}
			found_new_contact = true;
			float pen2 = distance2(pt_on_this_edge, pt_on_other_edge);
		} while (0);

		for (auto& game_object : game_objects) {
			game_object->init();
		}

	}

	void Scene_05::frame_start() {
		Scene_base::frame_start();
	}

	void Scene_05::update(float deltaTimeSeconds) {
		Scene_base::update(deltaTimeSeconds);
	}

	void Scene_05::frame_end() {
		Scene_base::frame_end();
	}

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

	void Scene_05::on_input_update(float deltaTime, int mods) {
	}

	void Scene_05::on_key_press(int key, int mods) {
		basic_bool_button_changer(key, mods);
	}

	void Scene_05::on_key_release(int key, int mods) {
		// add key release event
	}

	void Scene_05::on_mouse_move(int mouseX, int mouseY, int deltaX, int deltaY) {
		// add mouse move event
	}

	void Scene_05::on_mouse_btn_press(int mouseX, int mouseY, int button, int mods) {
		// add mouse button press event
	}

	void Scene_05::on_mouse_btn_release(int mouseX, int mouseY, int button, int mods) {
		// add mouse button release event
	}

	void Scene_05::on_mouse_scroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	}

	void Scene_05::on_window_resize(int width, int height) {
	}
}