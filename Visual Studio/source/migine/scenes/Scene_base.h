#pragma once

#include <migine/Game_objects/Game_object.h>
#include <migine/Contact_detection/BVH.h>
#include <migine/Camera.h>
#include <migine/Game_objects/Components/Transform.h>
#include <migine/Game_objects/Contact_detection/Rigid_body.h>

#include <Core/World.h>

#include <string>
#include <vector>
#include <type_traits>
#include <functional>
#include <memory>

namespace EngineComponents {
	class Transform;
}

class InputController;

namespace migine {

	class Scene_base : public World
	{
		friend class SceneInput;
	public:
		Scene_base();
		~Scene_base();// TODO elibereaza resursele

		template<class T>
		void register_game_object2(std::unique_ptr<T> game_object) {
			static_assert(std::is_base_of<Game_object, T>());
			if constexpr (std::is_base_of<Collider_base, T>()) {
				Collider_base* collider = static_cast<Collider_base*>(game_object.get());
				bvh.cache_contacts_and_insert(collider);
				colliders.push_back(collider);
			}
			if constexpr (std::is_base_of<Renderer_base, T>()) {
				renderers.push_back(static_cast<Renderer_base*>(game_object.get()));
			}
			if constexpr (std::is_base_of<Rigid_body, T>()) {
				rigid_bodies.push_back(static_cast<Rigid_body*>(game_object.get()));
			}
			game_objects.push_back(move(game_object));
		}
		void register_game_object(std::unique_ptr<Game_object> game_object);

		glm::vec3 light_position;
		glm::vec3 light_direction;
		// TODO use unique
		Camera* camera;

	protected:
		virtual void draw_coordinat_system();
		virtual void draw_coordinat_system(const glm::mat4& view_matrix, const glm::mat4& projection_maxtix);

		virtual void render_mesh(const Mesh& mesh, const Shader& shader, glm::vec3 position, glm::vec3 scale = glm::vec3(1));
		virtual void render_mesh(const Mesh& mesh, glm::vec3 position, glm::vec3 scale = glm::vec3(1));

		virtual void render_mesh_2d(const Mesh& mesh, const Shader& shader, const glm::mat3& model_matrix);
		virtual void render_mesh_2d(const Mesh& mesh, const glm::mat3& model_matrix, glm::vec3 color) const;

		virtual void render_mesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model_matrix);

		Camera& get_scene_camera() const;
		InputController& get_camera_input() const;

		BVH bvh;
		// memory managed by this
		// use unique. make it a const vector, as resizeing will invalidate all teh references... or use a list or something
		std::vector<std::unique_ptr<Game_object>> game_objects;
		std::vector<gsl::not_null<Renderer_base*>> renderers;
		std::vector<gsl::not_null<Rigid_body*>> rigid_bodies;
		std::vector<gsl::not_null<Collider_base*>> colliders;


	private:
		void init_resources();

		// memory managed by this
		InputController* camera_input;

		bool draw_ground_plane;
		// memory managed by this
		// TODO use unique or resource manager
		Mesh* xoz_plane;
		// memory managed by this
		// TODO use unique or resource manager
		Mesh* simple_line;
		// memory managed by this
		// TODO use unique
		EngineComponents::Transform* object_model;
	};
}