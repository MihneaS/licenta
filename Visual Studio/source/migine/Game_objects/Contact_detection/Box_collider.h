#pragma once

#include "Collider_base.h"
#include <migine/game_objects/components/Has_mesh.h>

#include <unordered_map>
#include <vector>

namespace migine {
	class Box_collider: public Collider_base, virtual public Has_mesh {
	public:
		Collision check_collision(const Collider_base& other) const override; // double dispatch T.T I hate it
		Collision check_collision(const Box_collider& other) const override;
		Collision check_collision(const Sphere_collider& other) const override;
		std::tuple<glm::vec3, glm::vec3> provide_aabb_parameters() const override;

	protected:
		Box_collider();

	private:
		void construct_local_corners();

		static inline std::unordered_map<Mesh_id, std::vector<glm::vec3> > cache; // inline??? probabil e bine, dar de ce?
		std::vector<glm::vec3> local_corners; // corners positions in local space.
		//TODO should I use std::array?
	};
}