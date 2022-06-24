#pragma once

#include <migine/constants.h>

//#include <migine/game_objects/physics/Rigid_body.h>

#include <gsl/gsl>

#include <vector>


namespace migine {
	class Rigid_body;

	class Force_generator_base {
	public:
		Force_generator_base() = default;
		Force_generator_base(const Force_generator_base&) = default;
		virtual ~Force_generator_base() = default;

		virtual void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) = 0;
		virtual std::unique_ptr<Force_generator_base> make_deep_copy() = 0;
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
		void remove(gsl::not_null<Rigid_body*> obj);
		void clear();
		void update_forces(float delta_time);

	private:
		std::vector<Entry> registrations; // TODO maybe use hashtable
	};

	class Gravity_generator : public Force_generator_base {
	public:
		Gravity_generator() = default;
		Gravity_generator(const Gravity_generator&) = default;
		~Gravity_generator() override = default;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		static glm::vec3 gravity;
	};

	class To_center_gravity_generator : public Force_generator_base {
	public:
		To_center_gravity_generator() = default;
		To_center_gravity_generator(const To_center_gravity_generator&) = default;
		~To_center_gravity_generator() override = default;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		static float gravity_magnitude;
	};

	class Drag_generator: public Force_generator_base {
	public:
		Drag_generator(float k1, float k2);
		Drag_generator(const Drag_generator&) = default;
		~Drag_generator() override = default;
		
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
	private:
		float k1;
		float k2;
	};

	class Test_bouyant_force_generator: public Force_generator_base {
	public:
		Test_bouyant_force_generator(const Test_bouyant_force_generator&) = default;
		~Test_bouyant_force_generator() override = default;

		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	};

	class Test_cos_force_generator: public Force_generator_base {
	public:
		Test_cos_force_generator(glm::vec3 axis = {0,1,0}, float multiplier = 1);
		Test_cos_force_generator(const Test_cos_force_generator&) = default;
		~Test_cos_force_generator() override = default;

		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	private:
		float total_time = 0;
		float multiplier;
		glm::vec3 axis;
	};

	class Linear_speed_generator: public Force_generator_base {
	public:
		Linear_speed_generator(float min_y, float max_y, float desired_speed);
		Linear_speed_generator(const Linear_speed_generator&) = default;
		~Linear_speed_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
	private:
		float min_y;
		float max_y;
		float desired_speed;
	};

	class Constant_torque_generator : public Force_generator_base {
	public:
		Constant_torque_generator(glm::vec3 torque);
		Constant_torque_generator(const Constant_torque_generator&) = default;
		~Constant_torque_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
	private:
		glm::vec3 torque;
	};

	class Sinusoidal_torque_generator : public Force_generator_base {
	public:
		Sinusoidal_torque_generator(glm::vec3 max_torque, float miu = 1);
		Sinusoidal_torque_generator(const Sinusoidal_torque_generator&) = default;
		~Sinusoidal_torque_generator() override = default;

		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
	private:
		glm::vec3 max_torque;
		float total_time = 0;
		float miu;
	};

	class Constant_force_generator : public Force_generator_base {
	public:
		Constant_force_generator(glm::vec3 force);
		Constant_force_generator(const Constant_force_generator&) = default;
		~Constant_force_generator() override = default;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
		glm::vec3 force;
	};

	class Sinusiodal_force_on_point_generator : public Force_generator_base {
	public:
		Sinusiodal_force_on_point_generator(glm::vec3 force, glm::vec3 point);
		Sinusiodal_force_on_point_generator(const Sinusiodal_force_on_point_generator&) = default;
		~Sinusiodal_force_on_point_generator() override = default;
		std::unique_ptr<Force_generator_base> make_deep_copy() override;
		void update_force(gsl::not_null<Rigid_body*> obj, float delta_time) override;
	private:
		glm::vec3 force;
		glm::vec3 point;
		float total_time;
	};

	//class Scene_base;

	//template<class Obj_t, class Scene_t>
	//std::vector<std::unique_ptr<Force_generator_base>> get_initial_force_generators() {
	//	static_assert(std::is_base_of<Rigid_body, Obj_t>());
	//	static_assert(std::is_base_of<Scene_base, Scene_t>());
	//	std::vector<std::unique_ptr<Force_generator_base>> ret;
	//	ret.push_back(make_unique<Gravity_generator>()); // DEMO1
		//ret.push_back(make_unique<Drag_generator>(0.5f, 0.5f));
		//ret.push_back(make_unique<Test_bouyant_force_generator>());
		//ret.push_back(make_unique<Test_cos_force_generator>());
	//	return ret;
	//}
}