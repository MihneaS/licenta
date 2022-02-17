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
		float get_mass() const;
		void set_mass(float mass);
		float get_inverse_mass() const;
		void set_inverse_mass(float inverse_mass);
		glm::vec3 get_constant_acceleration() const;
		const glm::mat3& get_inverse_invertia_tensor() const;
		glm::vec3 get_velocity() const;
		void add_velocity(glm::vec3 velocity);
		glm::quat get_angular_velocity() const;
		void add_angular_velocity(glm::vec3 rotation);
		glm::vec3 get_last_frame_acceleration() const;
		void add_force(glm::vec3 force);
		void add_force_at_point(glm::vec3 force, glm::vec3 point);
		void add_force_at_body_point(glm::vec3 force, glm::vec3 point);
		void add_torque(glm::vec3 torque);
		void clear_force_accum();
		void clear_torque_accum();
		void clear_accumulators();
		void calculate_derived_data();

	protected:
		void set_inertia_tensor(const glm::mat3& inerta_tensor);
		void set_inverse_inertia_tensor(const glm::mat3& inverse_inertia_tensor);
		virtual void compute_inverse_inertia_tensor() = 0; // TODO recompute when scale or mass change
		void compute_inverse_inertia_tensor_world();

	private:

		glm::vec3 velocity = k_vec3_zero;
		glm::vec3 angular_velocity = k_vec3_zero;
		glm::vec3 constant_acceleration = k_vec3_zero; // = k_default_gravity;
		glm::vec3 last_frame_acceleration = k_vec3_zero;
		//glm::vec3 last_frame_acceleration = k_vec3_zero;
		//glm::vec3 angular_acceleration = k_vec3_zero;
		glm::vec3 force_accumulator = k_vec3_zero;
		glm::vec3 torque_accumulator = k_vec3_zero;
		float inverse_mass = 0;
		glm::mat3 inverse_inertia_tensor = k_i3;
		glm::mat3 inverse_inertia_tensor_world = k_i3;
		float linear_damping = 0.995f;
		float angular_damping = 0.995f;
	};
}