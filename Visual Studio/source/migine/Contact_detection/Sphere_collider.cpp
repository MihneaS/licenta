#include "Box_collider.h"
#include "Sphere_collider.h"
#include <migine/Resource_manager.h>
#include <migine/constants.h>

#include <numeric>
#include <algorithm>
#include <cmath>

#include <migine/define.h>

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
using std::min;
using std::max;

namespace migine {
	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Collider_base& other) {
		if (is_asleep() && other.is_asleep()) {
			return vector<unique_ptr<Contact>>();
		}
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Box_collider& other) {  // DRY principle
		return other.check_collision(*this);
	}

	vector<unique_ptr<Contact>> Sphere_collider::check_collision(Sphere_collider& other) {
		vector<unique_ptr<Contact>> ret;
		float radii_sum = get_radius() + other.get_radius();
		vec3 this_center = get_center_world();
		vec3 other_center = other.get_center_world();
		float dist2 = distance2(this_center, other_center);
		if (dist2 < radii_sum * radii_sum) {
			vec3 this_to_other = other_center - this_center;
			vec3 contact_point = (other_center + this_center) / 2.0f;
			float pen_depth = radii_sum - sqrtf(dist2);
			ret.push_back(make_unique<Contact>(this, &other, contact_point, normalize(this_to_other), pen_depth));
#ifdef DEBUGGING
			(*ret.rbegin())->type = "sphere-sphere";
#endif // DEBUGGING
		}
		return ret;
	}

	tuple<vec3, vec3> Sphere_collider::provide_slim_aabb_parameters() const {
		vec3 center_world = get_center_world();
		vec3 min_pos = {center_world.x - radius, center_world.y - radius, center_world.z - radius};
		vec3 max_pos = {center_world.x + radius, center_world.y + radius, center_world.z + radius};
		return {min_pos, max_pos};
	}

	tuple<vec3, vec3> Sphere_collider::provide_fat_aabb_parameters() const {
		auto [min_pos, max_pos] = provide_slim_aabb_parameters();
		vec3 delta_pos = k_aabb_fattening_time * get_velocity();
		vec3 min_after_moving = min_pos + delta_pos;
		vec3 max_after_moving = max_pos + delta_pos;
		
		min_pos.x = min(min_pos.x, min_after_moving.x);
		min_pos.y = min(min_pos.y, min_after_moving.y);
		min_pos.z = min(min_pos.z, min_after_moving.z);

		max_pos.x = max(max_pos.x, max_after_moving.x);
		max_pos.y = max(max_pos.y, max_after_moving.y);
		max_pos.z = max(max_pos.z, max_after_moving.z);

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
