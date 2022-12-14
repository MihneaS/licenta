#pragma once

#include <migine/define.h>
#include <migine/game_objects/Game_object.h>
#include <migine/contact_detection/BVH.h>
#include <migine/Camera.h>
#include <migine/game_objects/components/Transform.h>
#include <migine/physics/Rigid_body.h>
#include <migine/physics/force_generators.h>
#include <migine/contact_detection/Contact.h>

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
	class Renderer_base;

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

		std::unique_ptr<Game_object> unregister_game_object(gsl::not_null<Game_object*> game_object);

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

		template<class Obj_t>
		std::unique_ptr<Game_object> _unregister_game_object(gsl::not_null<Obj_t*> game_object) {
			static_assert(std::is_base_of<Game_object, Obj_t>());
			if constexpr (std::is_base_of<Renderer_base, Obj_t>()) {
				gsl::not_null<Renderer_base*> renderer = static_cast<Renderer_base*>(game_object.get());
				for (auto it = renderers.begin(); it != renderers.end(); ++it) {
					if (*it == renderer.get()) {
						renderers.erase(it);
						break;
					}
				}
			}
			if constexpr (std::is_base_of<Rigid_body, Obj_t>()) {
				gsl::not_null<Rigid_body*> rigid_body = static_cast<Rigid_body*>(game_object.get());
				for (auto it = rigid_bodies.begin(); it != rigid_bodies.end(); ++it) {
					if (*it == rigid_body.get()) {
						rigid_bodies.erase(it);
						break;
					}
				}
				force_registry.remove(game_object);
			}
			if constexpr (std::is_base_of<Collider_base, Obj_t>()) {
				gsl::not_null<Collider_base*> collider = static_cast<Collider_base*>(game_object.get());
				bvh.remove(collider);
			}

			std::unique_ptr<Game_object> ret;
			for (auto it = game_objects.begin(); it != game_objects.end(); ++it) {
				if (it->get() == game_object.get()) {
					ret = move(*it);
					game_objects.erase(it);
					break;
				}
			}
			return ret;
		}

		template<class Obj_t>
		std::unique_ptr<Game_object> unregister_game_object(Obj_t* game_object) { // for convinience....
			return _unregister_game_object(gsl::not_null(game_object));
		}

		template <class Obj_t, typename... Params>
		gsl::not_null<Obj_t*> make_game_object(Params... params) {
			std::unique_ptr<Obj_t> new_obj = std::make_unique<Obj_t>(std::forward<Params>(params)...);
			Obj_t* ret_ptr = new_obj.get();
			register_game_object(move(new_obj)); // TODO use emplace
			return ret_ptr;
		}

		// deprecated
		void register_game_object2(std::unique_ptr<Game_object> game_object);

		void basic_bool_button_changer(int key, int mods);
		virtual void modify_contacts(std::vector<std::unique_ptr<Contact>>& contacts);
		const std::unordered_set<gsl::not_null<Collider_base*>> get_objects_in_contact_with(gsl::not_null<Collider_base*> collider) const;
		void spawn_walls(glm::vec3 pos_down_center, glm::vec3 enclosed_volume_scale);
		void spawn_walls_large();
		void spawn_walls_small();
		void spawn_balls(int n);
		void spawn_boxes(int n);
		void create_and_shoot_ball();

		glm::vec3 light_position;
		glm::vec3 light_direction;
		// TODO use unique
		std::unique_ptr<Camera> camera;
		bool see_bvh = true;
		bool see_walls = true;

	protected:
		virtual void draw_coordinat_system();
		virtual void draw_coordinat_system(const glm::mat4& view_matrix, const glm::mat4& projection_maxtix);
		virtual void draw_coordinat_system2();
		virtual void draw_coordinat_system2(const glm::mat4& view_matrix, const glm::mat4& projection_maxtix);

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
		bool run_one_frame = false;
		bool time_stopped = false;
		bool time_slowed = false;
		bool do_resolve_penetrations = true;
		bool do_resolve_velocities = true;
		bool destroy_objects_deep_down = true;
		int penetration_type = 1;

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