#pragma once

#include <glm/glm.hpp>
#include <gsl/gsl>

#include <vector>
#include <array>
#include <memory>

#include <migine/contact_detection/Contact.h>

namespace migine {
	class Contact_resolver {
	public:
		struct Additional_contact_data {
			glm::vec3 calculate_local_velocity(gsl::not_null<const Contact*> collision, int obj_idx, float delta_time);
			Additional_contact_data(gsl::not_null<const Contact*> collision, float delta_time);

			glm::quat world_to_contact_rotation;
			glm::quat contact_to_world_rotation;
			glm::vec3 contact_local_velocity;
			float desired_delta_velocity = 0;
			std::array<glm::vec3, 2> relative_contact_positions;
		};

		Contact_resolver(const std::vector<std::unique_ptr<Contact>>& contacts, float delta_time);

		void resolve_contacts(std::vector<std::unique_ptr<Contact>>& contacts);
		void resolve_penetrations(std::vector<std::unique_ptr<Contact>>& contacts);
		void resolve_velocity(std::vector<std::unique_ptr<Contact>>& contacts);
		void calculate_desired_delta_velocity(Contact& contact, Additional_contact_data& more_data, float delta_time);

	private:
		std::vector<Additional_contact_data> additional_contact_data;
		float delta_time;
	};
}