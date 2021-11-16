#pragma once

#include <Core/GPU/Mesh.h>
#include <Core/GPU/Shader.h>

#include <unordered_map>

namespace Migine {
	class ResourceManager {
	private:
		ResourceManager();

		std::unordered_map<MeshId, Mesh*> meshes;
		std::unordered_map<ShaderId, Shader*> shaders;
		
	public:
		void operator=(ResourceManager const&) = delete;
		static ResourceManager* GetInstance();
		Mesh* GetMesh(MeshId id) const;
		Shader* GetShader(ShaderId id) const;
		template<MeshId id> friend Mesh* GetMesh();
		template<ShaderId id> friend Shader* GetShader();
		template<MeshId id> void LoadMesh();
		template<ShaderId id> void LoadShader();
		void StoreMesh(MeshId id, Mesh* mesh);
		void StoreMesh(Mesh* mesh);
		void StoreShader(ShaderId id, Shader* shader);
		void StoreShader(Shader* shader);
		void ReloadShaders() const;
	};

	ResourceManager* GetResourceManager();
	template<MeshId id> Mesh* GetMesh() {
		ResourceManager* rm = ResourceManager::GetInstance();
		if (rm->meshes.find(id) == rm->meshes.end()) {
			rm->LoadMesh<id>();
		}
		return rm->GetMesh(id);
	}
	template<ShaderId id> Shader* GetShader() {
		ResourceManager* rm = ResourceManager::GetInstance();
		if (rm->shaders.find(id) == rm->shaders.end()) {
			rm->LoadShader<id>();
		}
		return rm->GetShader(id);
	}
}