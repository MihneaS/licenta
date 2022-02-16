#pragma once

#include <migine/game_objects/components/Has_transform.h>
#include <migine/game_objects/components/Has_name.h>
#include <migine/game_objects/components/Has_id.h>
#include <migine/contact_detection/Contact.h>
#include <migine/contact_detection/BVH.h>
#include <migine/game_objects/physics/Rigid_body.h>
#include <migine/define.h>

#include <vector>
#include <tuple>
#include <memory>

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
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Collider_base& other) = 0; // for double dispatch T.T which I hate
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Box_collider& other)  = 0;
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Sphere_collider& other) = 0;

		virtual std::tuple<glm::vec3, glm::vec3> provide_aabb_parameters() const = 0;

	private:
		// owned by the BVH using this collider 
		BVH::Node* bvh_node = nullptr;
	};
}