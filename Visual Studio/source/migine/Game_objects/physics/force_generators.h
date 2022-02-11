#pragma once

#include <migine/constants.h>

#include <gsl/gsl>

#include <vector>


namespace migine {
	class Rigid_body;

	class Force_generator_base {
	public:
		virtual ~Force_generator_base() = default;

		virtual void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) = 0;
	protected:
		glm::vec3 obtain_force_for_desired_velocity(gsl::not_null<Rigid_body*> obj, float delta_time, glm::vec3 desired_velocity);
	};

	class Force_registry {
	public:
		struct Entry {
			Entry(gsl::not_null<Rigid_body*> obj, std::unique_ptr<Force_generator_base> force_generator);

			gsl::not_null<Rigid_body*> obj;
			std::unique_ptr<Force_generator_base> force_generator;
		};

		void add(gsl::not_null<Rigid_body*> obj, std::unique_ptr<Force_generator_base> force_generator);
		void remove(gsl::not_null<Rigid_body*> obj, gsl::not_null<Force_generator_base*> force_generator);
		void clear();
		void update_forces(float delta_time);

	private:
		std::vector<Entry> registrations; // TODO maybe use hashtable
	};

	class Gravity_generator : public Force_generator_base {
	public:
		~Gravity_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		static glm::vec3 gravity;
	};

	class Drag_generator: public Force_generator_base {
	public:
		Drag_generator(float k1, float k2);
		~Drag_generator() override = default;
		
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	private:
		float k1;
		float k2;
	};

	class Test_bouyant_force_generator: public Force_generator_base {
	public:
		~Test_bouyant_force_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	};

	class Test_cos_force_generator: public Force_generator_base {
	public:
		Test_cos_force_generator(glm::vec3 axis = {0,1,0}, float multiplier = 1);
		~Test_cos_force_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	private:
		float total_time = 0;
		float multiplier;
		glm::vec3 axis;
	};

	class Linear_speed_generator: public Force_generator_base {
	public:
		Linear_speed_generator(float min_y, float max_y, float desired_speed);
		~Linear_speed_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;

	private:
		float min_y;
		float max_y;
		float desired_speed;
	};
}