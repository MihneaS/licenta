#include "force_generators.h"
#include <migine/game_objects/physics/Rigid_body.h>

#include <cmath>

using glm::vec3;
using glm::normalize;

using gsl::not_null;

using std::unique_ptr;
using std::make_unique;
using std::move;
using std::cos;

namespace migine {
	Force_registry::Entry::Entry(gsl::not_null<Rigid_body*> obj, std::unique_ptr<Force_generator_base> force_generator) :
		obj(obj), force_generator(move(force_generator)) {
	}

	void Force_registry::add(not_null<Rigid_body*> obj, unique_ptr<Force_generator_base> force_generator) {
		registrations.emplace_back(obj, move(force_generator));
	}

	void Force_registry::remove(gsl::not_null<Rigid_body*> obj, gsl::not_null<Force_generator_base*> force_generator) {
		for (auto it = registrations.begin(); it != registrations.end(); it++) {
			if (it->obj == obj && it->force_generator.get() == force_generator.get()) {
				registrations.erase(it);
				break;
			}
		}
	}

	void Force_registry::clear() {
		registrations.clear();
	}

	void Force_registry::update_forces(float delta_time) {
		for (auto& entry : registrations) {
			entry.force_generator->update_force(entry.obj, delta_time);
		}
	}

	vec3 Gravity_generator::gravity = k_default_gravity;

	unique_ptr<Force_generator_base> Gravity_generator::make_deep_copy() {
		return make_unique<Gravity_generator>(*this);
	}

	void Gravity_generator::update_force(not_null<Rigid_body*> obj, float delta_time) {
		obj->add_force(gravity * obj->get_inverse_mass());
	}

	Drag_generator::Drag_generator(float k1, float k2) :
		k1(k1), k2(k2) {
	}

	void Drag_generator::update_force(not_null<Rigid_body*> obj, float delta_time) {
		vec3 force = obj->get_velocity();
		float drag_coeff = (float)force.length();
		drag_coeff = k1 * drag_coeff + k2 * drag_coeff * drag_coeff;
		force = normalize(force);
		force *= -drag_coeff;
		obj->add_force(force);
	}

	unique_ptr<Force_generator_base> Drag_generator::make_deep_copy() {
		return make_unique<Drag_generator>(*this);
	}

	unique_ptr<Force_generator_base> Test_bouyant_force_generator::make_deep_copy() {
		return make_unique<Test_bouyant_force_generator>(*this);
	}

	void Test_bouyant_force_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		float y = obj->transform.get_world_position().y;
		static float multiplier = 3;
		if (y < -1) {
			obj->add_force(-multiplier*k_default_gravity);
		} else if (obj->transform.get_world_position().y < 0) {
			obj->add_force(-multiplier * y * k_default_gravity);
		}
	}

	Test_cos_force_generator::Test_cos_force_generator(glm::vec3 axis, float multiplier) :
		axis(normalize(axis)), multiplier(multiplier) {
	}

	unique_ptr<Force_generator_base> Test_cos_force_generator::make_deep_copy() {
		return make_unique<Test_cos_force_generator>(*this);
	}

	void Test_cos_force_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		total_time += delta_time;
		obj->add_force(multiplier * cos(total_time) * axis);
	}

	Linear_speed_generator::Linear_speed_generator(float min_y, float max_y, float desired_speed) :
		min_y(min_y), max_y(max_y), desired_speed(desired_speed) {
	}
	
	void Linear_speed_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		float y = obj->transform.get_world_position().y;
		float velocity_y = obj->get_velocity().y;
		float speed_target = desired_speed;
		if (y > max_y) {
			speed_target = -desired_speed;
		} else if (y < min_y) {
			speed_target = desired_speed;
		} else if (velocity_y > 0) {
			speed_target = desired_speed;
		} else {
			speed_target = -desired_speed;
		}
		obj->add_force(obtain_force_for_desired_velocity(obj, delta_time, {0, speed_target, 0}));
	}

	unique_ptr<Force_generator_base> Linear_speed_generator::make_deep_copy() {
		return make_unique<Linear_speed_generator>(*this);
	}
	
	vec3 Force_generator_base::obtain_force_for_desired_velocity(not_null<Rigid_body*> obj, float delta_time, vec3 desired_velocity) {
		return (desired_velocity - obj->get_velocity() - obj->get_constant_acceleration() * delta_time) / (delta_time * obj->get_inverse_mass());
	}
}