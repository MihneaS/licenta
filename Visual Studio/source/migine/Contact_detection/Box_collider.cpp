#include "Box_collider.h"
#include "Sphere_collider.h"
#include <migine/define.h>
#include <migine/Resource_manager.h>
#include <migine/make_array.h>
#include <migine/utils.h>
#include <migine/scenes/current_scene.h>
#include <migine/game_objects/shapes/Debug_point.h>

#include <algorithm>
#include <vector>
#include <numeric>
#include <array>
#include <climits>

#include <iostream>

#include <migine/define.h>

using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::mat3;
using glm::mat4;
using glm::inverse;
using glm::clamp;
using glm::distance2;
using glm::dot;
using glm::cross;
using glm::normalize;

using std::max;
using std::min;
using std::tuple;
using std::pair;
using std::vector;
using std::array;
using std::accumulate;
using std::unique_ptr;
using std::make_unique;
using std::make_pair;
using std::abs;
using std::numeric_limits;

namespace migine {
	vector<unique_ptr<Contact>> Box_collider::check_collision(Collider_base& other) {
		if (is_asleep() && other.is_asleep()) {
			return vector<unique_ptr<Contact>>();
		}
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Box_collider::check_collision(Box_collider& other) {
		// check intersection of projections on 15 axis
		if (!fast_do_overlap(other)) {
			return vector<unique_ptr<Contact>>();
		}

		// check for contacts between corners of this and faces of other
		vector<unique_ptr<Contact>> corner_contacts_1;
		float max_depth_1 = 0;
		auto this_corners = get_corners_world();
		for (auto& this_corner : this_corners) {
			if (auto collision = other.check_collision_point(this_corner, *this); collision) {
				max_depth_1 = max(max_depth_1, collision->penetration_depth);
				corner_contacts_1.push_back(move(collision));
#ifdef DEBUGGING
				(*corner_contacts_1.rbegin())->type = "box-box point-face";
#endif // DEBUGGING
			}
		}

		// check for contacts between corners of other and faces of this
		vector<unique_ptr<Contact>> corner_contacts_2;
		float max_depth_2 = 0;
		auto other_corners = other.get_corners_world();
		for (auto& others_corner : other_corners) {
			if (auto collision = check_collision_point(others_corner, other); collision) {
				max_depth_2 = max(max_depth_2, collision->penetration_depth);
				corner_contacts_2.push_back(move(collision));
#ifdef DEBUGGING
				(*corner_contacts_2.rbegin())->type = "box-box point-face";
#endif // DEBUGGING
			}
		}

		// keep deepest
		//if (max_depth_1 > max_depth_2) {
		//	ret = move(corner_contacts_1);
		//} else {
		//	ret = move(corner_contacts_2);
		//}

		// check for edge-edge contacts
		vector<unique_ptr<Contact>> edge_contacts;
		float max_depth_edges = 0;
		vec3 this_center = transform.get_world_position();
		vec3 other_center = other.transform.get_world_position();
		for (auto& [this_idx1, this_idx2] : get_edges_indexes_in_corners()) {
			float min_pen2 = numeric_limits<float>::infinity();
			vec3 saved_pt_on_this;
			vec3 saved_pt_on_other;
			bool found_new_contact = false;
			vec3 saved_backup_norm{0};
			//DEBUG
			array<vec3, 4> saved_corners;
			array<int, 4> saved_idxes;
			//DEBUG
			for (auto& [other_idx1, other_idx2] : other.get_edges_indexes_in_corners()) {
				auto& this_corner1 = this_corners[this_idx1];
				auto& this_corner2 = this_corners[this_idx2];
				auto& other_corner1 = other_corners[other_idx1];
				auto& other_corner2 = other_corners[other_idx2];
				auto [pt_on_this_edge, pt_on_other_edge, clipped] = get_closest_points_between_segments(this_corner1, this_corner2, other_corner1, other_corner2);
				static_assert(std::is_same<decltype(clipped), bool>());
				if (clipped) {
					continue;
				}
				assert(is_point_on_axis(this_corner1, this_corner2, pt_on_this_edge));
				assert(is_point_on_axis(other_corner1, other_corner2, pt_on_other_edge));
				//assert(is_point_on_axis2(this_corner1, this_corner2, pt_on_this_edge));
				//assert(is_point_on_axis2(other_corner1, other_corner2, pt_on_other_edge));

				if (!is_point_inside(pt_on_other_edge)) {
					continue;
				}
				if (!other.is_point_inside(pt_on_this_edge)) {
					continue;
				}
				found_new_contact = true;
				float pen2 = distance2(pt_on_this_edge, pt_on_other_edge);
				if (pen2 < min_pen2) {
					// DEBUG
					saved_corners = {this_corner1, this_corner2, other_corner1, other_corner2};
					saved_idxes = {this_idx1, this_idx2, other_idx1, other_idx2};
					// DEBUG
					min_pen2 = pen2;
					saved_pt_on_this = pt_on_this_edge;
					saved_pt_on_other = pt_on_other_edge;
					if (pen2 == 0) {
						vec3 this_edge_mid = mid_point(this_corners[this_idx1], this_corners[this_idx2]);
						vec3 this_center_to_edge = normalize(this_edge_mid - this_center);
						vec3 other_edge_mid = mid_point(other_corners[other_idx1], other_corners[other_idx2]);
						vec3 other_center_to_edge = normalize(other_edge_mid - other_center);
						saved_backup_norm = normalize(this_center_to_edge - other_center_to_edge);
#ifdef DEBUGGING
						std::cout << "\nbox-box edge-edge backup norm used\n";
#endif // DEBUGGING
					}
				}
			}
			if (found_new_contact) {
				vec3 norm{0};
				if (min_pen2 == 0) {
					norm = saved_backup_norm;
				} else {
					// note: saved point on this is closer to other's center and vice versa
					// therefore norm will point towards other (also, remember AB = OB - BA)
					norm = normalize(saved_pt_on_this - saved_pt_on_other);
				}
				float depth = sqrtf(min_pen2);
				max_depth_edges = max(max_depth_edges, depth);
				edge_contacts.push_back(make_unique<Contact>(
						this,
						&other, 
						mid_point(saved_pt_on_other, saved_pt_on_this), 
						norm, 
						depth));
#ifdef DEBUGGING
				(*edge_contacts.rbegin())->type = "box-box edge-edge";
#endif // DEBUGGING
			}
		}

		if (max_depth_1 > max_depth_2 && max_depth_1 > max_depth_edges) {
			return corner_contacts_1;
		} else if (max_depth_2 > max_depth_edges) {
			return corner_contacts_2;
		} else {
			return edge_contacts;
		}
	}

	vector<unique_ptr<Contact>> Box_collider::check_collision(Sphere_collider& other) {
		vector<unique_ptr<Contact>> ret;

		vec3 sphere_center = other.get_center_world();
		Transform t(transform.get_world_position(), vec3{1}, transform.get_orientation());
		vec3 scaled_half_side_lengths = half_side_lengths * transform.get_scale();
		vec3 rel_sphere_center = t.transform_to_local(sphere_center);
		if (abs(rel_sphere_center.x) - other.get_radius() > scaled_half_side_lengths.x ||
			abs(rel_sphere_center.y) - other.get_radius() > scaled_half_side_lengths.y ||
			abs(rel_sphere_center.z) - other.get_radius() > scaled_half_side_lengths.z) {
			return ret;
		}

		vec3 closest_point = clamp(rel_sphere_center, -scaled_half_side_lengths, scaled_half_side_lengths);
		float r = other.get_radius();
		vec3 closest_point_world = t.get_model() * position_to_vec4(closest_point);
		float dist2 = distance2(closest_point_world, sphere_center);
		if (dist2 > r * r) {
			return ret;
		}

		vec3 normal = sphere_center - closest_point_world;
		if (normal == k_vec3_zero) {
			normal = sphere_center - transform.get_world_position();
		}
		normal = normalize(normal);
		float pen_depth = r - sqrtf(dist2);
		ret.push_back(make_unique<Contact>(this, &other, closest_point_world, normal, pen_depth));
		//ret.push_back(make_unique<Contact>(&other, this, closest_point_world, -normal, pen_depth));
#ifdef DEBUGGING
		(*ret.rbegin())->type = "box-sphere";
#endif // DEBUGGING
		
		// TODO
		//contact->restitution = data->restitution; pg 309
		//contact->friction = data->friction;

		return ret;
	}

	tuple<vec3, vec3> Box_collider::provide_slim_aabb_parameters() const {
		vec3 min_pos, max_pos;
		auto corners = get_corners_world();
		min_pos = max_pos = corners[0];
		for (int i = 1; i < corners.size(); i++) {
			min_pos.x = min(min_pos.x, corners[i].x);
			min_pos.y = min(min_pos.y, corners[i].y);
			min_pos.z = min(min_pos.z, corners[i].z);
			max_pos.x = max(max_pos.x, corners[i].x);
			max_pos.y = max(max_pos.y, corners[i].y);
			max_pos.z = max(max_pos.z, corners[i].z);
		}
		return {min_pos, max_pos};
	}

	tuple<vec3, vec3> Box_collider::provide_fat_aabb_parameters() const {
		vec3 scaled_half_side_lenghts = transform.get_scale() * half_side_lengths; // TODO suppose element wise multiplication
		float dist_to_corner = length(scaled_half_side_lenghts);
		vec3 center_world = transform.get_model() * position_to_vec4(local_center);
		vec3 moved_center_world = center_world + k_aabb_fattening_time * get_velocity();

		vec3 min_pos;
		vec3 max_pos;

		min_pos.x = min(center_world.x, moved_center_world.x) - dist_to_corner;
		min_pos.y = min(center_world.y, moved_center_world.y) - dist_to_corner;
		min_pos.z = min(center_world.z, moved_center_world.z) - dist_to_corner;

		max_pos.x = max(center_world.x, moved_center_world.x) + dist_to_corner;
		max_pos.y = max(center_world.y, moved_center_world.y) + dist_to_corner;
		max_pos.z = max(center_world.z, moved_center_world.z) + dist_to_corner;

		return {min_pos, max_pos};
	}

	// inspired by get_closest_points_between_segemnts from https://github.com/godotengine/godot/blob/master/core/math/geometry_3d.h
	tuple<vec3, vec3, bool> Box_collider::get_closest_points_between_segments(vec3 seg0_p0, vec3 seg0_p1, vec3 seg1_p0, vec3 seg1_p1) const {
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
			vec3 mid_point = (seg0_p0 + seg0_p1 + seg1_p0 + seg1_p1 ) / 4.0f;
			vec3 res_p0 = project_point_onto_axis(mid_point, seg0_p0, seg0_p1);
			vec3 res_p1 = project_point_onto_axis(mid_point, seg1_p0, seg1_p1);
			bool clipped = res_p0 != mid_point || res_p1 != mid_point;
			return {res_p0, res_p1, clipped};
		}
	}

	Box_collider::Box_collider() : Has_mesh(get_mesh<Mesh_id::box>()) { // TODO nu e nevoie sa retii in fiecare obiect, doar intoarce din cache dupa ce ai construit
		if (auto cached_corners_iter = cache.find(mesh.get_id()); cached_corners_iter != cache.end()) {
			local_center = cached_corners_iter->second.local_center;
			half_side_lengths = cached_corners_iter->second.half_side_lengths;
		} else {
			vec3 min_pos, max_pos;
			min_pos = max_pos = mesh.positions[0];
			for (int i = 1; i < mesh.positions.size(); i++) {
				min_pos.x = min(min_pos.x, mesh.positions[i].x);
				max_pos.x = max(max_pos.x, mesh.positions[i].x);
				min_pos.y = min(min_pos.y, mesh.positions[i].y);
				max_pos.y = max(max_pos.y, mesh.positions[i].y);
				min_pos.z = min(min_pos.z, mesh.positions[i].z);
				max_pos.z = max(max_pos.z, mesh.positions[i].z);
			}
			local_center = (min_pos + max_pos) / 2.0f;
			half_side_lengths = (max_pos - min_pos) / 2.0f;
			cache.insert(make_pair(mesh.get_id(), Cache_entry(local_center, half_side_lengths))); // TODO emplace, maybe?
		}
	}

	float Box_collider::maximum_corner_projection_onto_axis(vec3 axis) const {
		return dot(glm::abs(axis), glm::abs(mat3(transform.get_model()) * half_side_lengths));
	}

	bool Box_collider::do_overlap_on_axis(const Box_collider& other, glm::vec3 axis) const {
		float this_maximum_projection = maximum_corner_projection_onto_axis(axis);
		float other_maximum_projection = other.maximum_corner_projection_onto_axis(axis);
		float projected_distance = abs(dot(transform.get_world_position() - other.transform.get_world_position(), axis));
		return projected_distance < this_maximum_projection + other_maximum_projection;
	}

	bool Box_collider::fast_do_overlap(const Box_collider& other) const {
		auto xyz_basis = make_array(vec3{1,0,0}, vec3{0,1,0}, vec3{0,0,1});
		for (const auto& axis : xyz_basis) {
			auto axis_other = mat3(transform.get_model()) * axis;
			if (do_overlap_on_axis(other, axis_other)) {
				return true;
			}
		}
		for (const auto& axis1 : xyz_basis) {
			auto axis_this = mat3(transform.get_model()) * axis1;
			if (do_overlap_on_axis(other, axis_this)) {
				return true;
			}
			for (const auto& axis2 : xyz_basis) {
				auto axis_other = mat3(transform.get_model()) * axis2;
				auto axis_composed = cross(axis_this, axis_other);
				if (do_overlap_on_axis(other, axis_composed)) {
					return true;
				}
			}
		}
		return false;
	}

	std::unique_ptr<Contact> Box_collider::check_collision_point(glm::vec3 point, Collider_base& other) {
		unique_ptr<Contact> ret = nullptr;
		vec3 relative_point = transform.transform_to_local(point);

		vec3 depths;
		array<vec3, 3> normals;
		depths[0] = abs(transform.get_scale().x) * (half_side_lengths.x - abs(relative_point.x));
		if (depths[0] < 0) {
			return ret;
		}
		normals[0] = normalize(transform.get_axis<Axis::ox>() * ((relative_point.x - local_center.x < 0) ? -1.0f : 1.0f));

		depths[1] = abs(transform.get_scale().y) * (half_side_lengths.y - abs(relative_point.y));
		if (depths[1] < 0) {
			return ret;
		}
		normals[1] = normalize(transform.get_axis<Axis::oy>() * ((relative_point.y - local_center.y < 0) ? -1.0f : 1.0f));

		depths[2] = abs(transform.get_scale().z) * (half_side_lengths.z - abs(relative_point.z));
		if (depths[2] < 0) {
			return ret;
		}
		normals[2] = normalize(transform.get_axis<Axis::oz>() * ((relative_point.z - local_center.z < 0) ? -1.0f : 1.0f));

		vec3 relative_other_center = transform.transform_to_local(other.transform.get_world_position());
		vec3 contact_point_to_other_center_local = relative_other_center - relative_point;
		vec3 contact_point_to_this_center_local = local_center - relative_point;

		constexpr vec3 std_half_len = vec3{0.5};
		
		vec3 direction = normalize(other.transform.get_orientation() * -copy_sing_element_wise(vec3{1}, other.transform.transform_to_local(point)));
		vec3 relative_point_to_standardised_other_center = length(std_half_len) * direction;
		vec3 standardised_other_center = relative_point + relative_point_to_standardised_other_center;
		assert(local_center == vec3{0} && half_side_lengths == vec3{0.5});
		vec3 standardised_this_center = local_center;

		// observation: relative point from standardised centers is canceled out in the subtraction below
		vec3 standardised_this_center_to_other_center = standardised_other_center - standardised_this_center;

		float x_projection = abs(standardised_this_center_to_other_center.x);
		float y_projection = abs(standardised_this_center_to_other_center.y);
		float z_projection = abs(standardised_this_center_to_other_center.z);

		int best_axis = -1;
		if (x_projection > y_projection && x_projection > z_projection) {
			best_axis = 0;
		} else if (y_projection > z_projection) {
			best_axis = 1;
		} else {
			best_axis = 2;
		}

		ret = make_unique<Contact>(this, &other, point, normals[best_axis], depths[best_axis]);
		return ret;
	}

	bool Box_collider::is_point_inside(vec3 point) const {
		vec3 relative_point = transform.transform_to_local(point);

		return (half_side_lengths.x - abs(relative_point.x)) >= 0 &&
		       (half_side_lengths.y - abs(relative_point.y)) >= 0 &&
		       (half_side_lengths.z - abs(relative_point.z)) >= 0;
	}

	Box_collider::Cache_entry::Cache_entry(glm::vec3 local_center, glm::vec3 half_side_lenghts) :
	local_center(local_center), half_side_lengths(half_side_lenghts) {
	}
}
