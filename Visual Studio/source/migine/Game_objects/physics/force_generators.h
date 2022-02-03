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
}