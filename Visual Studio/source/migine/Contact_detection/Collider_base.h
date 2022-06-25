#pragma once

#include <migine/game_objects/components/Has_spcon_transform.h>
#include <migine/game_objects/components/Has_name.h>
#include <migine/game_objects/components/Has_id.h>
#include <migine/game_objects/components/Has_mesh.h>
#include <migine/contact_detection/Contact.h>
#include <migine/physics/Rigid_body.h>
#include <migine/define.h>

#include <vector>
#include <tuple>
#include <memory>

namespace migine {
	class Box_collider;
	class Sphere_collider;

	class Collider_base : virtual public Has_id, public Rigid_body, virtual public Has_mesh
#ifdef DEBUGGING 
		, virtual public Has_name
#endif // DEBUGGING
	{

	public:
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Collider_base& other) = 0; // for double dispatch T.T which I hate
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Box_collider& other)  = 0;
		virtual std::vector<std::unique_ptr<Contact>> check_collision(Sphere_collider& other) = 0;

		virtual std::tuple<glm::vec3, glm::vec3> provide_slim_aabb_parameters() const = 0;
		virtual std::tuple<glm::vec3, glm::vec3> provide_fat_aabb_parameters() const = 0;

		bool is_in_contact_with_other_static();
		bool is_static();
	};
}