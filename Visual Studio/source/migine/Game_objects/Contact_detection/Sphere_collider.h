#pragma once

#include "Collider_base.h"
#include <migine/game_objects/components/Has_mesh.h>

namespace migine {
	class Sphere_collider: public Collider_base, virtual public Has_mesh {
	public:
		Collision check_collision(const Collider_base& other) const override; // double dispatch T.T I hate it
		Collision check_collision(const Box_collider& other) const override;
		Collision check_collision(const Sphere_collider& other) const override;

		virtual std::tuple<glm::vec3, glm::vec3> provide_aabb_parameters() const override;

		float get_diameter();
	protected:
		Sphere_collider();

	private:
		void compute_center_and_radius();

		glm::vec3 center;
		float radius;

		static inline std::unordered_map<Mesh_id, std::tuple<glm::vec3, float>> cache;
	};
}