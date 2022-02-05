#include "Transform.h"

#include <migine/utils.h>

using glm::mat4;
using glm::mat3;
using glm::vec3;
using glm::vec4;
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
		world_position(position), scale(scale),  orientation(rotation) {
		internal_update();
	}

	void Transform::change_state_with_delta (vec3 delta_pos, vec3 relative_scale_change, vec3 delta_rot) {
		world_position += delta_pos;
		scale *= relative_scale_change;
		orientation = quat_add_vec3(orientation, delta_rot);

		compute_world_model();
	}

	void Transform::change_state(vec3 new_pos, vec3 new_scale, quat new_rot) {
		world_position = new_pos;
		scale = new_scale;
		orientation = new_rot;

		internal_update();
	}

	void Transform::change_state_with_delta(vec3 delta_pos, vec3 delta_rot) {
		change_state_with_delta(delta_pos, {1,1,1}, delta_rot);
	}

	void Transform::change_state(vec3 new_pos, quat new_rot) {
		change_state(new_pos, scale, new_rot);
	}

	void Transform::compute_world_model() {
		world_model = translate(glm::mat4(1.0f), world_position) *
			toMat4(orientation) *
			glm::scale(glm::mat4(1.0f), scale);
		//world_model = translate(toMat4(orientation) * glm::scale(mat4(1.0f), scale), world_position);
	}

	vec3 Transform::get_world_position() const {
		return world_position;
	}

	vec3 Transform::get_scale() const {
		return scale;
	}

	quat Transform::get_orientation() const {
		return orientation;
	}

	const mat4& Transform::get_model() const {
		return world_model;
	}
	vec3 Transform::get_point_in_world_space(vec3 point) const {
		return world_model * position_to_vec4(point);
	}
	void Transform::change_position_with_delta(vec3 delta_pos) {
		world_position += delta_pos;
	}
	void Transform::change_orientation_with_delta(vec3 rotation) {
		orientation = quat_add_vec3(orientation, rotation);
	}
	void Transform::internal_update() {
		orientation /= orientation.length(); // TODO poate mai rar
		compute_world_model();
	}
}