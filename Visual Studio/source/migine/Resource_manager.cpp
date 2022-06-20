#include "Resource_manager.h"

#include <Core/Engine.h>

#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::endl;
using std::make_pair;
using std::pair;
using std::make_unique;
using std::unique_ptr;
using std::move;

namespace migine {
	Resource_manager& Resource_manager::get_instance() {
		static Resource_manager instance;
		return instance;
	}

	const Mesh& Resource_manager::get_mesh(Mesh_id id) const {
		return *meshes.at(id).get();
	}
	const Shader& Resource_manager::get_shader(Shader_id id) const {
		return *shaders.at(id).get();
	}

	void Resource_manager::store_mesh(std::unique_ptr<Mesh> mesh) {
		meshes.emplace(mesh->get_id(), move(mesh));
	}

	void Resource_manager::store_shader(std::unique_ptr<Shader> shader) {
		shaders.emplace(shader->get_id(), move(shader));
	}

	//void Resource_manager::reload_shaders() const {
	//	cout << "\n";
	//	cout << "=============================\n";
	//	cout << "Reloading Shaders\n";
	//	cout << "=============================\n";
	//	cout << "\n";

	//	for (auto& shader : shaders) {
	//		shader.second.Reload();
	//	}
	//}

	Resource_manager& get_resource_manager() {
		return Resource_manager::get_instance();
	}

	template<> void Resource_manager::load_mesh<Mesh_id::box>() {
		unique_ptr<Mesh> mesh = make_unique<Mesh>(Mesh_id::box);
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		store_mesh(move(mesh));
	}

	template<> void Resource_manager::load_mesh<Mesh_id::box_wireframe>() {
		unique_ptr<Mesh> mesh = make_unique<Mesh>(Mesh_id::box_wireframe);
		mesh->SetDrawMode(GL_LINES);
		vector<VertexFormat> vertices;
		vertices.reserve(8);
		vector<unsigned int> indices;
		indices.reserve(24);
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f));
		vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f));
		vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f));
		vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f));
		vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f));
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(3);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(4);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(1);
		indices.push_back(5);
		indices.push_back(2);
		indices.push_back(6);
		indices.push_back(3);
		indices.push_back(7);
		mesh->InitFromData(vertices, indices);
		store_mesh(move(mesh));
	}

	template<> void Resource_manager::load_mesh<Mesh_id::plane>() {
		unique_ptr<Mesh> mesh = make_unique<Mesh>(Mesh_id::plane);
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		store_mesh(move(mesh));
	}

	template<> void Resource_manager::load_mesh<Mesh_id::sphere>() {
		unique_ptr<Mesh> mesh = make_unique<Mesh>(Mesh_id::sphere);
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		store_mesh(move(mesh));
	}

	template<> void Resource_manager::load_shader<Shader_id::vertex_color>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::vertex_color);
		shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "VertexColor.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}

	template<> void Resource_manager::load_shader<Shader_id::vertex_normal>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::vertex_normal);
		shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "Normals.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}

	template<> void Resource_manager::load_shader<Shader_id::color>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::color);
		shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "Color.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}

	template<> void Resource_manager::load_shader<Shader_id::simple>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::simple);
		shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "Default.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}

	template<> void Resource_manager::load_shader<Shader_id::lab8>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::lab8);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}

	template<> void Resource_manager::load_shader<Shader_id::sphere>() {
		unique_ptr<Shader> shader = make_unique<Shader>(Shader_id::sphere);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/MVP.texture_sphere.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/Normals.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		store_shader(move(shader));
	}
}