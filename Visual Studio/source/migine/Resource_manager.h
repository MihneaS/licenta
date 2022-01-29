#pragma once

#include <Core/GPU/Mesh.h>
#include <Core/GPU/Shader.h>

#include <unordered_map>
#include <memory>

namespace migine {
	class Resource_manager {
	public:
		void operator=(Resource_manager const&) = delete;
		static Resource_manager& get_instance();
		const Mesh& get_mesh(Mesh_id id) const;
		const Shader& get_shader(Shader_id id) const;
		template<Mesh_id id> friend const Mesh& get_mesh();
		template<Shader_id id> friend const Shader& get_shader();
		template<Mesh_id id> void load_mesh();
		template<Shader_id id> void load_shader();
		void store_mesh(std::unique_ptr<Mesh> mesh);
		void store_shader(std::unique_ptr<Shader> shader);
		//void reload_shaders() const;
	private:
		Resource_manager() = default;

		std::unordered_map<Mesh_id, std::unique_ptr<const Mesh>> meshes;
		std::unordered_map<Shader_id, std::unique_ptr<const Shader>> shaders;

	};

	Resource_manager& get_resource_manager();
	// TODO get by filepath
	template<Mesh_id id> const Mesh& get_mesh() {
		Resource_manager& rm = Resource_manager::get_instance();
		if (rm.meshes.find(id) == rm.meshes.end()) {
			rm.load_mesh<id>();
		}
		return rm.get_mesh(id);
	}
	// TODO get by filepath
	template<Shader_id id> const Shader& get_shader() {
		Resource_manager& rm = Resource_manager::get_instance();
		if (rm.shaders.find(id) == rm.shaders.end()) {
			rm.load_shader<id>();
		}
		return rm.get_shader(id);
	}
}