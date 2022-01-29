#include "Transform.h"

using glm::mat4;
using glm::vec3;
using glm::quat;
using glm::toMat4;

/*
namespace migine {
	Transform::Transform(const vec3& position, const vec3& scale, const quat& rotation) {
		world_model = glm::scale(world_model, scale);
		rotate(rotation);
		translate(position);
	}
	
	void Transform::translate(const vec3& delta_position) {
		world_model = glm::translate(world_model, delta_position);
	}

	void Transform::rotate(const quat& delta_rotation) {
		world_model = toMat4(delta_rotation) * world_model;
	}

}
*/

namespace migine {
	Transform::Transform(vec3 position, vec3 scale, glm::quat rotation) :
		world_position(position), scale(scale),  world_rotation(rotation) {
		compute_world_model();
	}

	void Transform::change_state_with_delta (vec3 delta_pos, vec3 relative_scale_change, glm::quat delta_rot) {
		world_position += delta_pos;
		scale *= relative_scale_change;
		world_rotation *= delta_rot;

		compute_world_model();
	}

	void Transform::change_state(vec3 new_pos, vec3 new_scale, glm::quat new_rot) {
		world_position = new_pos;
		scale = new_scale;
		world_rotation = new_rot;

		compute_world_model();
	}

	void Transform::compute_world_model() {
		world_model = translate(glm::mat4(1.0f), world_position) *
			toMat4(world_rotation) *
			glm::scale(glm::mat4(1.0f), scale);
		//world_model = translate(toMat4(world_rotation) * glm::scale(mat4(1.0f), scale), world_position);
	}

	vec3 Transform::get_world_position() const {
		return world_position;
	}

	vec3 Transform::get_scale() const {
		return scale;
	}

	quat Transform::get_world_rotation() const {
		return world_rotation;
	}

	const mat4& Transform::get_model() const {
		return world_model;
	}

}