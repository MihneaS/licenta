#pragma once

#include <migine/Game_objects/Components/Has_transform.h>
#include <migine/Game_objects/Components/Has_name.h>
#include <migine/Game_objects/Components/Has_id.h>
#include <migine/Contact_detection/Collision.h>
#include <migine/Contact_detection/BVH.h>
#include <migine/Game_objects/Contact_detection/Rigid_body.h>
#include <migine/define.h>

#include <tuple>
#include <string>

namespace migine {
	class Box_collider;
	class Sphere_collider;

	class Collider_base : virtual public Has_transform, virtual public Has_id, public Rigid_body 
#ifdef DEBUGGING 
		, virtual public Has_name
#endif // DEBUGGING
	{
		friend class BVH;

	public:
		virtual Collision check_collision(const Collider_base& other) const = 0; // for double dispatch T.T which I hate
		virtual Collision check_collision(const Box_collider& other) const = 0;
		virtual Collision check_collision(const Sphere_collider& other) const = 0;

		virtual std::tuple<glm::vec3, glm::vec3> provide_aabb_parameters() const = 0;

	private:
		// owned by the BVH using this collider 
		BVH::Node* bvh_node = nullptr;
	};
}