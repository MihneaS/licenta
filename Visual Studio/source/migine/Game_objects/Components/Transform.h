#pragma once

//#include <Component/Transform/Transform.h>
#include <include/glm.h>

#include <migine/constants.h>
/*
namespace migine {
	class Transform {
	public:
		explicit Transform(const glm::vec3& position = {0, 0, 0}, const glm::vec3& scale = {1, 1, 1}, const glm::quat& orientation = glm::quat());
		Transform(const Transform& transform) = default;

		const glm::mat4& get_model() const;
		void translate(const glm::vec3& delta_position);
		void rotate(const glm::quat& delta_rotation);

	private:
		glm::mat4 world_model = glm::mat4(1.0f);
	};
}
*/



namespace migine {
	//using Transform = EngineComponents::Transform;
	class Transform {
	public:
		explicit Transform(glm::vec3 position = k_vec3_zero, glm::vec3 scale = {1, 1, 1}, glm::quat orientation = glm::quat());
		Transform(const Transform& transform) = default;

		glm::vec3 get_world_position() const;
		glm::vec3 get_scale() const;
		glm::quat get_orientation() const;
		const glm::mat4& get_model() const;
		glm::vec3 get_point_in_world_space(glm::vec3 point) const;
		void change_position_with_delta(glm::vec3 delta_pos);
		void change_orientation_with_delta(glm::vec3 rotation);
		void change_state(glm::vec3 new_pos = k_vec3_zero, glm::vec3 new_scale = {1, 1, 1}, glm::quat new_rot = glm::quat());
		void internal_update();
		glm::vec3 transform_to_local(glm::vec3 point) const;
		template <Axis axis> glm::vec3 get_axis() const;

	private:
		void compute_world_model();

		glm::mat4 world_model = glm::mat4(1.0f);
		glm::vec3 world_position = {0, 0, 0}; // TODO elimina aceasta parte deoarece exista aceasta informatie nealterata in wolrd_model[3]
		glm::vec3 scale = {1, 1, 1};
		glm::quat orientation = glm::quat();

		glm::vec3 get_axis(int axis) const;
		// old - deprecated 
		void change_state_with_delta(glm::vec3 delta_pos, glm::vec3 relative_scale_change, glm::vec3 delta_rot);
		void change_state_with_delta(glm::vec3 delta_pos, glm::vec3 delta_rot);
		void change_state(glm::vec3 new_pos = k_vec3_zero, glm::quat new_rot = glm::quat());
	};
}