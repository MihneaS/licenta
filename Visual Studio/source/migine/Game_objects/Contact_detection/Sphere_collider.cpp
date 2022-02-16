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
using glm::distance2;
using glm::normalize;

using std::vector;
using std::tuple;
using std::get;
using std::accumulate;
using std::max;
using std::unique_ptr;
using std::make_unique;

namespace migine {
	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Collider_base& other) {
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Box_collider& other) {  // DRY principle
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Sphere_collider& other) {
		vector<unique_ptr<Contact>> ret;
		float radii_sum = get_radius() + other.get_radius();
		float dist2 = distance2(center, other.center);
		if (dist2 < radii_sum * radii_sum) {
			vec3 this_to_other = other.center - center;
			vec3 contact_point = this_to_other / 2.0f;
			float pen_depth = radii_sum - sqrtf(dist2);
			ret.push_back(make_unique<Contact>(this, &other, contact_point, normalize(this_to_other), pen_depth));
		}
		return ret;
	}

	tuple<vec3, vec3> Sphere_collider::provide_aabb_parameters() const {
		vec3 center_world = transform.get_model() * vec4 { center, 1 };
		vec3 min_pos = {center_world.x - radius, center_world.y - radius, center_world.z - radius};
		vec3 max_pos = {center_world.x + radius, center_world.y + radius, center_world.z + radius};
		return {min_pos, max_pos};
	}

	float Sphere_collider::get_diameter() const { // TODO get radius*2 mayyybeeee
		vec3 scale = transform.get_scale();
		assert(scale.x == scale.y && scale.y == scale.z);
		return scale.x;
	}

	float Sphere_collider::get_radius() const {
		return radius;
	}
	
	vec3 Sphere_collider::get_center_world() const {
		return get_center_local() + transform.get_world_position();
	}

	vec3 Sphere_collider::get_center_local() const {
		return center;
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
