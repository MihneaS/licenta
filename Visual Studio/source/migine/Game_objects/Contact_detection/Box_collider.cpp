#include "Box_collider.h"
#include "Sphere_collider.h"
#include <migine/Resource_manager.h>

#include <algorithm>
#include <vector>
#include <numeric>

using glm::vec3;
using glm::vec4;
using glm::quat;
using std::max;
using std::min;
using std::tuple;
using std::pair;
using std::vector;
using std::accumulate;

namespace migine {
	Collision Box_collider::check_collision(const Collider_base& other) const {
		return other.check_collision(*this);
	}

	Collision Box_collider::check_collision(const Box_collider& other) const {
		assert(false); // TODO implementeaza
		return Collision();
	}

	Collision Box_collider::check_collision(const Sphere_collider& other) const { // DRY principle
		return other.check_collision(*this);
	}

	tuple<vec3, vec3> Box_collider::provide_aabb_parameters() const {
		auto& to_world_coords = transform.get_model();
		vec3 min_pos, max_pos;
		min_pos = max_pos = to_world_coords * vec4(local_corners[0], 1);
		for (int i = 0; i < local_corners.size(); i++) {
			vec3 world_corner = to_world_coords * vec4(local_corners[i], 1);
			min_pos.x = min(min_pos.x, world_corner.x);
			min_pos.y = min(min_pos.y, world_corner.y);
			min_pos.z = min(min_pos.z, world_corner.z);
			max_pos.x = max(max_pos.x, world_corner.x);
			max_pos.y = max(max_pos.y, world_corner.y);
			max_pos.z = max(max_pos.z, world_corner.z);
		}
		return {min_pos, max_pos};
	}

	void Box_collider::construct_local_corners() {
		if (auto cached_corners_iter = cache.find(mesh.get_id()); cached_corners_iter != cache.end()) {
			local_corners = cached_corners_iter->second;
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
			vec3 center = (min_pos + max_pos) / 2.0f;
			float half_side_length = max(max(max_pos.x - min_pos.x, max_pos.y - min_pos.y), max_pos.z - min_pos.z) / 2.0f;
			local_corners.reserve(8);
			local_corners = {
				center + vec3({ -half_side_length, -half_side_length, -half_side_length }),
				center + vec3({ +half_side_length, -half_side_length, -half_side_length }),
				center + vec3({ -half_side_length, +half_side_length, -half_side_length }),
				center + vec3({ +half_side_length, +half_side_length, -half_side_length }),
				center + vec3({ -half_side_length, -half_side_length, +half_side_length }),
				center + vec3({ +half_side_length, -half_side_length, +half_side_length }),
				center + vec3({ -half_side_length, +half_side_length, +half_side_length }),
				center + vec3({ +half_side_length, +half_side_length, +half_side_length })
			};
			cache[mesh.get_id()] = local_corners;
		}
	}

	Box_collider::Box_collider() : Has_mesh(get_mesh<Mesh_id::box>()) {
		construct_local_corners();
	}
}