#include "Box_collider.h"
#include "Sphere_collider.h"
#include <migine/Resource_manager.h>

#include <numeric>
#include <algorithm>
#include <cmath>

using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::distance;
using std::tuple;
using std::get;
using std::accumulate;
using std::max;

namespace migine {
	Collision Sphere_collider::check_collision(const Collider_base& other) const {
		return other.check_collision(*this);
	}

	Collision Sphere_collider::check_collision(const Box_collider& other) const {
		assert(false); // TODO implementeaza
		return Collision();
	}

	Collision Sphere_collider::check_collision(const Sphere_collider& other) const {
		assert(false); // TODO implementeaza
		return Collision();
	}

	tuple<vec3, vec3> Sphere_collider::provide_aabb_parameters() const {
		vec3 center_world = transform.get_model() * vec4 { center, 1 };
		vec3 min_pos = {center_world.x - radius, center_world.y - radius, center_world.z - radius};
		vec3 max_pos = {center_world.x + radius, center_world.y + radius, center_world.z + radius};
		return {min_pos, max_pos};
	}

	float Sphere_collider::get_diameter() {
		vec3 scale = transform.get_scale();
		assert(scale.x == scale.y && scale.y == scale.z);
		return scale.x;
	}

	Sphere_collider::Sphere_collider() : Has_mesh(get_mesh<Mesh_id::sphere>()) {
		compute_center_and_radius();
	}

	void Sphere_collider::compute_center_and_radius() {
		if (auto cached_corners_iter = cache.find(mesh.get_id()); cached_corners_iter != cache.end()) {
			center = get<vec3>(cached_corners_iter->second);
			radius = get<float>(cached_corners_iter->second);
		} else {
			center = accumulate(mesh.positions.begin(), mesh.positions.end(), vec3{0,0,0});
			radius = accumulate(mesh.positions.begin(), mesh.positions.end(), 0.0f,
				[=](float accumulator, vec3 point) { return max(accumulator, distance(center, point)); });
			cache[mesh.get_id()] = {center, radius};
		}
	}
}
