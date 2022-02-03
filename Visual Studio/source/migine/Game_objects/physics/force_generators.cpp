#include "force_generators.h"
#include <migine/game_objects/physics/Rigid_body.h>

using glm::vec3;
using glm::normalize;

using gsl::not_null;

using std::unique_ptr;
using std::move;

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

	void Gravity_generator::update_force(not_null<Rigid_body*> obj, float delta_time) {
		obj->add_force(gravity * obj->get_mass());
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
}