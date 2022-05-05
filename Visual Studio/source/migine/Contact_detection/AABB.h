#pragma once

//#include <migine/game_objects/components/Transform.h>
#include <migine/game_objects/components/Mesh.h>
#include <migine/define.h>

#include <gsl/gsl>

#ifdef DEBUGGING
#include <migine/game_objects/rendering/Wireframe_renderer.h>
#endif

namespace migine {
	class Collider_base;
	class Transform;

	class AABB {
	public:
		AABB(std::tuple<glm::vec3, glm::vec3> min_pos_max_pos);
		AABB(glm::vec3 min_pos, glm::vec3 max_pos);
		AABB(gsl::not_null<const Collider_base*> collider);
		AABB(const Mesh& mesh, const Transform& transform);
		AABB(AABB child0, AABB child1);

		float get_volume() const;
		glm::vec3 get_center() const;
		void enlarge_by(AABB to_enlarge_by);
		void resize(AABB child0, AABB child1);
		bool does_intersect(AABB other) const;
		bool contains(glm::vec3 point) const;

#ifdef DEBUGGING
		void render(const Camera& camera);
#endif

	private:
		glm::vec3 min_pos;
		glm::vec3 max_pos;

#ifdef DEBUGGING
		Wireframe_renderer renderer;
#endif
	};
}