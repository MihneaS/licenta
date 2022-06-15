#pragma once

#include "Collider_base.h"
#include <migine/game_objects/components/Has_mesh.h>

namespace migine {
	class Sphere_collider: public Collider_base, virtual public Has_mesh {
	public:
		std::vector<std::unique_ptr<Contact>> check_collision(Collider_base& other) override; // double dispatch T.T I hate it
		std::vector<std::unique_ptr<Contact>> check_collision(Box_collider& other) override;
		std::vector<std::unique_ptr<Contact>> check_collision(Sphere_collider& other) override;

		virtual std::tuple<glm::vec3, glm::vec3> provide_slim_aabb_parameters() const override;
		virtual std::tuple<glm::vec3, glm::vec3> provide_fat_aabb_parameters() const override;

		float get_diameter() const;
		float get_radius() const;
		glm::vec3 get_center_world() const;
		glm::vec3 get_center_local() const;
	protected:
		Sphere_collider();

	private:
		void compute_center_and_radius();

		glm::vec3 center;
		float radius;

		static inline std::unordered_map<Mesh_id, std::tuple<glm::vec3, float>> cache;
	};
}