#include "Box_collider.h"
#include "Sphere_collider.h"
#include <migine/Resource_manager.h>
#include <migine/make_array.h>

#include <algorithm>
#include <vector>
#include <numeric>
#include <array>
#include <climits>

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
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Box_collider::check_collision(Box_collider& other) {
		vector<unique_ptr<Contact>> ret;

		// check intersection of projections on 15 axis
		if (!fast_do_overlap(other)) {
			return ret;
		}

		// check for contacts between corners of this and faces of other
		vector<unique_ptr<Contact>> tmp1;
		float max_depth_1 = 0;
		auto this_corners = get_corners();
		for (auto& this_corner : this_corners) {
			if (auto collision = other.check_collision_point(this_corner, *this); collision) {
				max_depth_1 = max(max_depth_1, collision->penetration_depth);
				tmp1.push_back(move(collision));
#ifdef DEBUGGING
				(*tmp1.rbegin())->type = "box-box point-face";
#endif // DEBUGGING
			}
		}

		// check for contacts between corners of other and faces of this
		vector<unique_ptr<Contact>> tmp2;
		float max_depth_2 = 0;
		auto other_corners = other.get_corners();
		for (auto& others_corner : other_corners) {
			if (auto collision = check_collision_point(others_corner, other); collision) {
				max_depth_2 = max(max_depth_2, collision->penetration_depth);
				tmp2.push_back(move(collision));
#ifdef DEBUGGING
				(*tmp2.rbegin())->type = "box-box point-face";
#endif // DEBUGGING
			}
		}

		// keep deepest
		if (max_depth_1 > max_depth_2) {
			ret = move(tmp1);
		} else {
			ret = move(tmp2);
		}

		// check for edge-edge contacts
		vec3 this_center = transform.get_world_position();
		vec3 other_center = other.transform.get_world_position();
		for (auto& [this_idx1, this_idx2] : get_edges_indexes_in_corners()) {
			float min_pen2 = numeric_limits<float>::infinity();
			vec3 saved_pt_on_this;
			vec3 saved_pt_on_other;
			bool found_new_contact = false;
			for (auto& [other_idx1, other_idx2] : other.get_edges_indexes_in_corners()) {
				found_new_contact = false;
				auto [pt_on_this_edge, pt_on_other_edge, clipped] = get_closest_points_between_segments(this_corners[this_idx1], this_corners[this_idx2], other_corners[other_idx1], other_corners[other_idx2]);
				static_assert(std::is_same<decltype(clipped), bool>());
				if (clipped) {
					continue;
				}
				float dist2_to_contact_other = distance2(other_center, pt_on_this_edge);
				float dist2_to_edge_other = distance2(other_center, pt_on_other_edge);
				if (dist2_to_contact_other > dist2_to_edge_other) {
					continue;
				}
				float dist2_to_contact_this = distance2(this_center, pt_on_other_edge);
				float dist2_to_edge_this = distance2(this_center, pt_on_this_edge);
				if (dist2_to_contact_this > dist2_to_edge_this) {
					continue;
				}
				found_new_contact = true;
				float pen2 = distance2(pt_on_this_edge, pt_on_other_edge);
				if (pen2 < min_pen2) {
					min_pen2 = pen2;
					saved_pt_on_this = pt_on_this_edge;
					saved_pt_on_other = pt_on_other_edge;
				}
			}
			if (found_new_contact) {
				ret.push_back(make_unique<Contact>(
						this,
						&other, 
						mid_point(saved_pt_on_other, saved_pt_on_this), 
						saved_pt_on_other - saved_pt_on_this, 
						sqrtf(min_pen2)));
#ifdef DEBUGGING
				(*ret.rbegin())->type = "box-box edge-edge";
#endif // DEBUGGING
			}
		}

		return ret;
	}

	vector<unique_ptr<Contact>> Box_collider::check_collision(Sphere_collider& other) {
		vector<unique_ptr<Contact>> ret;

		vec3 sphere_center = other.get_center_world();
		vec3 rel_sphere_center = transform.transform_to_local(sphere_center);
		if (abs(rel_sphere_center.x) - other.get_radius() > half_side_lengths.x ||
			abs(rel_sphere_center.y) - other.get_radius() > half_side_lengths.y ||
			abs(rel_sphere_center.z) - other.get_radius() > half_side_lengths.z) {
			return ret;
		}

		vec3 closest_point = clamp(rel_sphere_center, -half_side_lengths, half_side_lengths); // TODO sigur face ce trebuie clamp?
		float r = other.get_radius();
		vec3 closest_point_world = transform.get_model() * position_to_vec4(closest_point);
		float dist2 = distance2(closest_point_world, sphere_center);
		if (dist2 > r * r) {
			return ret;
		}

		vec3 normal = normalize(sphere_center - closest_point_world);
		float pen_depth = r - sqrtf(dist2);
		ret.push_back(make_unique<Contact>(this, &other, closest_point_world, normal, pen_depth));
#ifdef DEBUGGING
		(*ret.rbegin())->type = "box-sphere";
#endif // DEBUGGING
		
		// TODO
		//contact->restitution = data->restitution; pg 309
		//contact->friction = data->friction;

		return ret;
	}

	tuple<vec3, vec3> Box_collider::provide_aabb_parameters() const {
		vec3 min_pos, max_pos;
		auto corners = get_corners();
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

	tuple<vec3, vec3, bool> Box_collider::get_closest_points_between_segments(vec3 seg0_p0, vec3 seg0_p1, vec3 seg1_p0, vec3 seg1_p1) const {
		// Do the function 'd' as defined by pb. I think it's a dot product of some sort.
//#define d_of(m, n, o, p) ((m.x - n.x) * (o.x - p.x) + (m.y - n.y) * (o.y - p.y) + (m.z - n.z) * (o.z - p.z))
#define d_of(v1, v2, v3, v4) = dot(v1-v2, v3-v4)

		// Calculate the parametric position on the 2 curves, mua and mub.
		//float mua = (d_of(seg0_p0, seg1_p0, seg1_p1, seg1_p0) * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0) - d_of(seg0_p0, seg1_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0)) / (d_of(seg0_p1, seg0_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0) - d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0) * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0));
		//float mub = (d_of(seg0_p0, seg1_p0, seg1_p1, seg1_p0) + mua * d_of(seg1_p1, seg1_p0, seg0_p1, seg0_p0)) / d_of(seg1_p1, seg1_p0, seg1_p1, seg1_p0);
#undef d_of

		float mua = (dot(seg0_p0-seg1_p0, seg1_p1-seg1_p0)*dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0) - dot(seg0_p0-seg1_p0, seg0_p1-seg0_p0)*dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0)) / 
			(dot(seg0_p1-seg0_p0, seg0_p1-seg0_p0)*dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0) - dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0) * dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0));
		float mub = (dot(seg0_p0-seg1_p0, seg1_p1-seg1_p0) + mua*dot(seg1_p1-seg1_p0, seg0_p1-seg0_p0)) / dot(seg1_p1-seg1_p0, seg1_p1-seg1_p0);

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
		for (const auto& axis1 : xyz_basis) {
			auto axis_this = mat3(transform.get_model()) * axis1;
			if (do_overlap_on_axis(other, axis_this)) {
				return true;
			}
			for (const auto& axis2 : xyz_basis) {
				auto axis_other = mat3(transform.get_model()) * axis2;
				if (do_overlap_on_axis(other, axis_other)) {
					return true;
				}
				auto axis_composed = cross(axis_this, axis_other);
				if (do_overlap_on_axis(other, axis_composed)) {
					return true;
				}
			}
		}
		return false;
	}

	unique_ptr<Contact> Box_collider::check_collision_point(vec3 point, Collider_base& other) {
		unique_ptr<Contact> ret = nullptr;
		vec3 relative_point = transform.transform_to_local(point);
		vec3 normal;

		float min_depth = half_side_lengths.x - abs(relative_point.x);
		if (min_depth < 0) {
			return ret;
		}
		normal = normalize(transform.get_axis<Axis::ox>() * ((relative_point.x < 0) ? -1.0f : 1.0f));
		
		float depth = half_side_lengths.y - abs(relative_point.y);
		if (depth < 0) {
			return ret;
		} else if (depth < min_depth) {
			min_depth = depth;
			normal = normalize(transform.get_axis<Axis::oy>() * ((relative_point.y < 0) ? -1.0f : 1.0f));
		}

		depth = half_side_lengths.z - abs(relative_point.z);
		if (depth < 0) {
			return ret;
		} else if (depth < min_depth) {
			min_depth = depth;
			normal = normalize(transform.get_axis<Axis::oz>() * ((relative_point.z < 0) ? -1.0f : 1.0f));
		}

		ret = make_unique<Contact>(this, &other, point, normal, min_depth); // TODO ugly second this! repiar imediatly after call
		return ret;
	}

	Box_collider::Cache_entry::Cache_entry(glm::vec3 local_center, glm::vec3 half_side_lenghts) :
	local_center(local_center), half_side_lengths(half_side_lenghts) {
	}
}