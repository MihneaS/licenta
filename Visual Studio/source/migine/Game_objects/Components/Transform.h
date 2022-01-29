#pragma once

//#include <Component/Transform/Transform.h>
#include <include/glm.h>

/*
namespace migine {
	class Transform {
	public:
		explicit Transform(const glm::vec3& position = {0, 0, 0}, const glm::vec3& scale = {1, 1, 1}, const glm::quat& rotation = glm::quat());
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
		explicit Transform(glm::vec3 position = {0, 0, 0}, glm::vec3 scale = {1, 1, 1}, glm::quat rotation = glm::quat());
		Transform(const Transform& transform) = default;

		void change_state_with_delta(glm::vec3 delta_pos, glm::vec3 relative_scale_change, glm::quat delta_rot);
		void change_state(glm::vec3 new_pos = {0, 0, 0}, glm::vec3 new_scale = {1, 1, 1}, glm::quat new_rot = glm::quat());
		glm::vec3 get_world_position() const;
		glm::vec3 get_scale() const;
		glm::quat get_world_rotation() const;
		const glm::mat4& get_model() const;

	private:
		void compute_world_model();

		glm::mat4 world_model = glm::mat4(1.0f);
		glm::vec3 world_position = {0, 0, 0};
		glm::vec3 scale = {1, 1, 1};
		glm::quat world_rotation = glm::quat();
	};

}