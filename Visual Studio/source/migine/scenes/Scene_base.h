#pragma once

#include <migine/define.h>
#include <migine/game_objects/Game_object.h>
#include <migine/contact_detection/BVH.h>
#include <migine/Camera.h>
#include <migine/game_objects/components/Transform.h>
#include <migine/physics/Rigid_body.h>
#include <migine/physics/force_generators.h>

#include <Core/World.h>

#include <string>
#include <vector>
#include <type_traits>
#include <functional>
#include <memory>
#include <utility>

namespace EngineComponents {
	class Transform;
}

class InputController;

namespace migine {
	class Spcon_transform;

	class Scene_base : public World
	{
		friend class SceneInput;
		friend class Spcon_transform;
	public:
		Scene_base();
		~Scene_base();// TODO elibereaza resursele

		void frame_start() override;
		void update(float delta_time_seconds) override;
		void frame_end() override;

		template<class Obj_t>
		void register_game_object(std::unique_ptr<Obj_t> game_object) {
			static_assert(std::is_base_of<Game_object, Obj_t>());
			if constexpr (std::is_base_of<Renderer_base, Obj_t>()) {
				renderers.push_back(static_cast<Renderer_base*>(game_object.get()));
			}
			if constexpr (std::is_base_of<Rigid_body, Obj_t>()) {
				gsl::not_null<Rigid_body*> rigid_body = static_cast<Rigid_body*>(game_object.get());
				rigid_bodies.push_back(rigid_body);
				auto& initial_force_generators = get_initial_force_generators(rigid_body);
				for (auto& force_generator : initial_force_generators) {
					force_registry.add(rigid_body, force_generator->make_deep_copy());
				}
			}
			if constexpr (std::is_base_of<Collider_base, Obj_t>()) {
				gsl::not_null<Collider_base*> collider = static_cast<Collider_base*>(game_object.get());
				bvh.insert(collider);
			}
			game_objects.push_back(move(game_object));
		}

		// deprecated
		void register_game_object2(std::unique_ptr<Game_object> game_object);

		glm::vec3 light_position;
		glm::vec3 light_direction;
		// TODO use unique
		std::unique_ptr<Camera> camera;

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
		Force_registry force_registry;

		std::vector<std::unique_ptr<Force_generator_base>> default_fs_gen;

	private:
		void init_resources();
		std::vector<std::unique_ptr<Force_generator_base>>& get_initial_force_generators(gsl::not_null<Rigid_body*> r_body);

		// memory managed by this
		std::unique_ptr<InputController> camera_input;

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