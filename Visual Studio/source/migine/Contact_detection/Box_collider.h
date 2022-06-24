#pragma once

#include "Collider_base.h"
#include <migine/game_objects/components/Has_mesh.h>
#include <migine/make_array.h>

#include <unordered_map>
#include <vector>
#include <tuple>

namespace migine {
	class Box_collider: public Collider_base{
		struct Cache_entry {
			Cache_entry(glm::vec3 local_center, glm::vec3 half_side_lenghts);

			glm::vec3 local_center;
			glm::vec3 half_side_lengths;
		};

	public:
		std::vector<std::unique_ptr<Contact>> check_collision(Collider_base& other) override; // double dispatch T.T I hate it
		std::vector<std::unique_ptr<Contact>> check_collision(Box_collider& other) override;
		std::vector<std::unique_ptr<Contact>> check_collision(Sphere_collider& other) override;
		std::tuple<glm::vec3, glm::vec3> provide_slim_aabb_parameters() const override;
		std::tuple<glm::vec3, glm::vec3> provide_fat_aabb_parameters() const override;
		// credits godot :https://github.com/godotengine/godot/blob/master/core/math/geometry_3d.h line 40
		std::tuple<glm::vec3, glm::vec3, bool> get_closest_points_between_segments(glm::vec3 seg1_pt1, glm::vec3 seg1_pt2, glm::vec3 seg2_pt1, glm::vec3 seg2_pt2) const;

	protected:
		Box_collider();

	private:
		// void construct_local_corners();
		float maximum_corner_projection_onto_axis(glm::vec3 axis) const;
		bool do_overlap_on_axis(const Box_collider& other, glm::vec3 axis) const;
		bool fast_do_overlap(const Box_collider& other) const;
		std::unique_ptr<Contact> check_collision_point(glm::vec3 point, Collider_base& other);
		bool is_point_inside(glm::vec3 point) const;
		auto get_corners_world() const { // TODO force inline maybe??? if this is not inlined, std::vector is better
			auto corners = make_array(
				local_center + glm::vec3({-half_side_lengths.x, -half_side_lengths.y, -half_side_lengths.z}), //0
				local_center + glm::vec3({+half_side_lengths.x, -half_side_lengths.y, -half_side_lengths.z}), //1
				local_center + glm::vec3({-half_side_lengths.x, +half_side_lengths.y, -half_side_lengths.z}), //2
				local_center + glm::vec3({+half_side_lengths.x, +half_side_lengths.y, -half_side_lengths.z}), //3
				local_center + glm::vec3({-half_side_lengths.x, -half_side_lengths.y, +half_side_lengths.z}), //4
				local_center + glm::vec3({+half_side_lengths.x, -half_side_lengths.y, +half_side_lengths.z}), //5
				local_center + glm::vec3({-half_side_lengths.x, +half_side_lengths.y, +half_side_lengths.z}), //6
				local_center + glm::vec3({+half_side_lengths.x, +half_side_lengths.y, +half_side_lengths.z})  //7
			);
			for (auto& corner : corners) {
				corner = transform.get_model() * position_to_vec4(corner);
			}
			return corners;
		}
		auto get_edges_indexes_in_corners() const {  // TODO force inline maybe??? if this is not inlined, std::vector is better // TODO 2 refferences instead of values ffs
			return make_array(
				std::make_tuple(0, 1),
				std::make_tuple(0, 2),
				std::make_tuple(0, 4),
				std::make_tuple(1, 3),
				std::make_tuple(1, 5),
				std::make_tuple(2, 3),
				std::make_tuple(2, 6),
				std::make_tuple(3, 7),
				std::make_tuple(4, 5),
				std::make_tuple(4, 6),
				std::make_tuple(5, 6),
				std::make_tuple(5, 7),
				std::make_tuple(6, 7)
			);
		}
		
		static inline std::unordered_map<Mesh_id, Cache_entry> cache; // inline??? probabil e bine, dar de ce?
		//std::vector<glm::vec3> local_corners; // corners positions in local space.
		//TODO should I use std::array?

		glm::vec3 local_center;
		glm::vec3 half_side_lengths;
	};
}
