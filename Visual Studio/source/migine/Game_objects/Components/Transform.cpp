#include "Transform.h"

#include <migine/utils.h>

using glm::mat4;
using glm::mat3;
using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::toMat4;
using glm::rotate;

namespace migine {
	Transform::Transform(vec3 position, vec3 scale, glm::quat orientation) :
		world_position(position), scale(scale),  orientation(orientation) {
		assert(is_finite(position));
		assert(is_finite(scale));
		assert(is_finite(orientation));
		internal_update();
	}

	void Transform::change_state_with_delta (vec3 delta_pos, vec3 relative_scale_change, vec3 delta_rot) {
		assert(is_finite(delta_pos));
		assert(is_finite(relative_scale_change));
		assert(is_finite(delta_rot));
		world_position += delta_pos;
		scale *= relative_scale_change;
		change_orientation_with_delta(delta_rot);

		internal_update();
	}

	void Transform::change_state(vec3 new_pos, vec3 new_scale, quat new_rot) {
		assert(is_finite(new_pos));
		assert(is_finite(new_scale));
		assert(is_finite(new_rot));
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
		assert(is_finite(delta_pos));
		world_position += delta_pos;
		internal_update();
	}

	void Transform::change_orientation_with_delta(vec3 rotation) {
		assert(is_finite(rotation));
		//orientation *= euler_angles_to_quat(rotation);
		float len = length(rotation);
		if (len == 0) {
			return;
		}
		quat tmp = rotate(quat(), len, rotation / len);
		orientation = tmp * orientation;
		//orientation = rotate(orientation, len,  rotation / len);
		internal_update();
	}

	void Transform::internal_update() {
		// orientation /= orientation.length(); // this line is wrong, it reduces the orientation  // TODO poate mai rar
		compute_world_model();
	}

	glm::vec3 Transform::transform_to_local(glm::vec3 point) const {
		return inverse(get_model()) * position_to_vec4(point);
	}

	vec3 Transform::rotate_and_scale_to_local(vec3 point) const {
		return inverse(toMat4(orientation) * glm::scale(glm::mat4(1), scale)) * position_to_vec4(point);
	}

	glm::vec3 Transform::get_axis(int axis) const {
		return get_model()[axis];
	}

	template<> glm::vec3 Transform::get_axis<Axis::ox>() const {
		return get_axis(0);
	}

	template<> glm::vec3 Transform::get_axis<Axis::oy>() const {
		return get_axis(1);
	}

	template<> glm::vec3 Transform::get_axis<Axis::oz>() const {
		return get_axis(2);
	}
}
