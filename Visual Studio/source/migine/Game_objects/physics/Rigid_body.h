#pragma once

#include <migine/game_objects/components/Has_transform.h>
#include <migine/game_objects/physics/force_generators.h>

#include <Core/Engine.h>

#include <vector>

#include <gsl/gsl>

namespace migine {
	class Force_generator_base;

	class Rigid_body : virtual public Has_transform {
	public:
		bool integrate(float delta_time);
		float get_mass();
		void set_mass(float mass);
		float get_inverse_mass();
		void set_inverse_mass(float inverse_mass);
		glm::vec3 get_velocity();
		glm::quat get_angular_velocity();
		void add_force(glm::vec3 force);
		void reset_forces();

	private:
		glm::vec3 velocity = {0, 0, 0};
		glm::quat angular_velocity = glm::quat();
		glm::vec3 force_acumulator = {0, 0, 0};
		float inverse_mass = 0;
	};
}