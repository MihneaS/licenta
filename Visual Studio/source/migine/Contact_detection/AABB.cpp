#include "AABB.h"

#include <migine/Game_objects/Contact_detection/Collider_base.h>
#include <Migine/Resource_manager.h>

#include <cassert>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;

using std::vector;
using std::min;
using std::max;
using std::tuple;

using gsl::not_null;

namespace migine {

	AABB::AABB(tuple<vec3, vec3> min_pos_max_pos) :
		AABB(get<0>(min_pos_max_pos), get<1>(min_pos_max_pos)) {
	}

	AABB::AABB(vec3 min_pos, vec3 max_pos) :
		min_pos(min_pos), max_pos(max_pos)
#ifdef DEBUGGING
		, renderer(get_shader<Shader_id::color>(), get_mesh<Mesh_id::box_wireframe>())
#endif
	{
		assert(min_pos.x <= max_pos.x);
		assert(min_pos.y <= max_pos.y);
		assert(min_pos.z <= max_pos.z);
	}

	AABB::AABB(not_null<const Collider_base*> collider) :
		AABB(collider->provide_aabb_parameters()) {
	}


	AABB::AABB(const Mesh& mesh, const Transform& transform)
#ifdef DEBUGGING
		: renderer(get_shader<Shader_id::color>(), get_mesh<Mesh_id::box_wireframe>())
#endif
	{
		vector<vec4> points;
		points.reserve(mesh.positions.size());
		for (auto& p : mesh.positions) {
			points.emplace_back(transform.get_model() * vec4(p.x, p.y, p.z, 1));
		}
		min_pos = max_pos = points[0];
		for (int i = 1; i < points.size(); i++) {
			min_pos.x = min(min_pos.x, points[i].x);
			max_pos.x = max(max_pos.x, points[i].x);
			min_pos.y = min(min_pos.y, points[i].y);
			max_pos.y = max(max_pos.y, points[i].y);
			min_pos.z = min(min_pos.z, points[i].z);
			max_pos.z = max(max_pos.z, points[i].z);
		}
	}

	AABB::AABB(AABB child0, AABB child1)
#ifdef DEBUGGING
		: renderer(get_shader<Shader_id::color>(), get_mesh<Mesh_id::box_wireframe>())
#endif
	{
		resize(child0, child1);
	}

	float AABB::get_volume() const {
		vec3 tmp = max_pos - min_pos;
		return tmp.x * tmp.y * tmp.z;
	}

	glm::vec3 AABB::get_center() const {
		return (min_pos + max_pos) * 0.5f;/// 2.0f;
	}

	void AABB::enlarge_by(AABB to_enlarge_by) {
		resize(*this, to_enlarge_by);
	}

	void AABB::resize(AABB child0, AABB child1) {
		min_pos.x = min(child0.min_pos.x, child1.min_pos.x);
		max_pos.x = max(child0.max_pos.x, child1.max_pos.x);
		min_pos.y = min(child0.min_pos.y, child1.min_pos.y);
		max_pos.y = max(child0.max_pos.y, child1.max_pos.y);
		min_pos.z = min(child0.min_pos.z, child1.min_pos.z);
		max_pos.z = max(child0.max_pos.z, child1.max_pos.z);
	}

	bool AABB::does_intersect(AABB other) const {
#ifdef DEBBUGGING
		BVH::aabb_intersection_operations_count++;
#endif
		return (min_pos.x <= other.max_pos.x && max_pos.x >= other.min_pos.x) &&
			(min_pos.y <= other.max_pos.y && max_pos.y >= other.min_pos.y) &&
			(min_pos.z <= other.max_pos.z && max_pos.z >= other.min_pos.z);
	}

	bool AABB::contains(glm::vec3 point) const {
		return min_pos.x < point.x&& point.x < max_pos.x&&
			min_pos.y < point.y&& point.y < max_pos.y&&
			min_pos.z < point.z&& point.z < max_pos.z;
	}


#ifdef DEBUGGING
	void AABB::render(const Camera& camera) {
		vec3 pos = get_center();
		vec3 scale = max_pos - min_pos;
		renderer.transform.change_state(pos, scale, quat());
		//transform.SetWorldPosition((aabb->minPos + aabb->maxPos) / 2.0f);
		//transform.SetScale(aabb->maxPos - aabb->minPos);
		renderer.render(camera);
	}
#endif

}