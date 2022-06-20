#include "Collider_base.h"
#include <migine/scenes/current_scene.h>

namespace migine {

	bool Collider_base::is_in_contact_with_other_static() {
		Scene_base& current_scene = get_current_scene();
		for (auto& other : current_scene.get_objects_in_contact_with(this)) { // TODO ineficient, nu ar trebui sa parcurg un for aici
			if (other->is_static()) {
				return true;
			}
		}
		return false;
	}
	bool Collider_base::is_static() {
		return get_inverse_mass() == 0 || is_asleep();
	}
}