#include "force_generators.h"
#include <migine/physics/Rigid_body.h>

#include <cmath>

using glm::vec3;
using glm::normalize;

using gsl::not_null;

using std::unique_ptr;
using std::make_unique;
using std::move;
using std::cos;
using std::remove_if;

namespace migine {
	Force_registry::Entry::Entry(gsl::not_null<Rigid_body*> obj, std::unique_ptr<Force_generator_base> force_generator) :
		obj(obj), force_generator(move(force_generator)) {
	}

	void Force_registry::add(not_null<Rigid_body*> obj, unique_ptr<Force_generator_base> force_generator) {
		registrations.emplace_back(obj, move(force_generator));
	}

	void Force_registry::remove(gsl::not_null<Rigid_body*> obj, gsl::not_null<Force_generator_base*> force_generator) {
		registrations.erase(remove_if(registrations.begin(), registrations.end(),
		                              [&](Entry& e) { return e.obj.get() == obj.get() && e.force_generator.get() == force_generator.get();}),
		                    registrations.end());
	}

	void Force_registry::remove(gsl::not_null<Rigid_body*> obj) {
		registrations.erase(remove_if(registrations.begin(), registrations.end(),
		                              [&](Entry& e) { return e.obj.get() == obj.get(); }),
		                    registrations.end());
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
		float drag_coeff = length(force);
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

	Constant_torque_generator::Constant_torque_generator(glm::vec3 torque) :
		torque(torque) {
	}

	unique_ptr<Force_generator_base> Constant_torque_generator::make_deep_copy() {
		return make_unique<Constant_torque_generator>(*this);
	}

	void Constant_torque_generator::update_force(not_null<Rigid_body*> obj, float delta_time) {
		//obj->add_torque(torque * obj->get_inverse_invertia_tensor());
		obj->add_torque(torque);
	}
	Sinusoidal_torque_generator::Sinusoidal_torque_generator(glm::vec3 max_torque, float miu) :
		max_torque(max_torque), miu(miu) {
	}

	void Sinusoidal_torque_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		total_time += delta_time;
		obj->add_torque(max_torque * cos(miu * total_time));
	}

	unique_ptr<Force_generator_base> Sinusoidal_torque_generator::make_deep_copy() {
		return make_unique<Sinusoidal_torque_generator>(*this);
	}

	Constant_force_generator::Constant_force_generator(vec3 force) :
		force(force) {
	}

	std::unique_ptr<Force_generator_base> Constant_force_generator::make_deep_copy() {
		return make_unique<Constant_force_generator>(*this);
	}

	void Constant_force_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		obj->add_force(force);
	}


	Sinusiodal_force_on_point_generator::Sinusiodal_force_on_point_generator(glm::vec3 force, glm::vec3 point) :
		force(force), point(point) {
	}

	std::unique_ptr<Force_generator_base> Sinusiodal_force_on_point_generator::make_deep_copy() {
		return make_unique<Sinusiodal_force_on_point_generator>(*this);
	}

	void Sinusiodal_force_on_point_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		total_time += delta_time;
		obj->add_force_at_body_point(force * cos(total_time), point);
	}

	std::unique_ptr<Force_generator_base> To_center_gravity_generator::make_deep_copy() {
		return make_unique<To_center_gravity_generator>(*this);
	}

	float To_center_gravity_generator::gravity_magnitude = glm::length(k_default_gravity);

	void To_center_gravity_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		vec3 direction = glm::normalize(vec3{0} - obj->transform.get_world_position());
		obj->add_force(direction * gravity_magnitude);
	}

	float To_points_gravity_generator::gravity_magnitude = glm::length(k_default_gravity);
	To_points_gravity_generator::To_points_gravity_generator(int power_n, float side_len) {
		vec3 min_pos{-(power_n - 1) * side_len / 2.0f};
		for (int i = 0; i < power_n; i++) {
			for (int j = 0; j < power_n; j++) {
				for (int k = 0; k < power_n; k++) {
					points.push_back(min_pos + side_len * vec3{i,j,k});
				}
			}
		}
	}

	std::unique_ptr<Force_generator_base> To_points_gravity_generator::make_deep_copy() {
		return make_unique<To_points_gravity_generator>(*this);
	}
	void To_points_gravity_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		vec3* closest_point = nullptr;
		float min_dist2 = std::numeric_limits<float>::max();
		for (auto& p : points) {
			float d2 = glm::distance2(obj->transform.get_world_position(), p);
			if (d2 < min_dist2) {
				min_dist2 = d2;
				closest_point = &p;
			}
		}
		vec3 direction = glm::normalize(*closest_point - obj->transform.get_world_position());
		obj->add_force(direction * gravity_magnitude);
	}

	float In_out_gravity_generator::gravity_magnitude = glm::length(k_default_gravity);
	In_out_gravity_generator::In_out_gravity_generator(float in_limit, float out_limit) :
			in_limit(in_limit), out_limit(out_limit) {
	}
	std::unique_ptr<Force_generator_base> In_out_gravity_generator::make_deep_copy() {
		return make_unique<In_out_gravity_generator>(*this);
	}
	void In_out_gravity_generator::update_force(gsl::not_null<Rigid_body*> obj, float delta_time) {
		vec3 obj_to_center = vec3{0} - obj->transform.get_world_position();
		float d2_to_center = glm::length2(obj_to_center);
		if (to_center) {
			if (d2_to_center < in_limit * in_limit) {
				to_center = false;
			}
		} else {
			if (d2_to_center > out_limit * out_limit) {
				to_center = true;
			}
		}
		vec3 force = obj_to_center / sqrtf(d2_to_center) * gravity_magnitude;
		if (!to_center) {
			force *= -1;
		}
		obj->add_force(force);
	}
}