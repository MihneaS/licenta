#include "ResourceManager.h"

#include <Core/Engine.h>

#include <iostream>
#include <vector>

using namespace Migine;
using std::vector;
using std::cout;
using std::endl;

ResourceManager::ResourceManager() {
}

ResourceManager* ResourceManager::GetInstance() {
	static ResourceManager instance;
	return &instance;
}

Mesh* ResourceManager::GetMesh(MeshId id) const {
	return meshes.at(id);
}
Shader* ResourceManager::GetShader(ShaderId id) const {
	return shaders.at(id);
}
void ResourceManager::StoreMesh(MeshId id, Mesh* mesh) {
	meshes[id] = mesh;
}
void ResourceManager::StoreMesh(Mesh* mesh) {
	StoreMesh(mesh->GetId(), mesh);
}
void ResourceManager::StoreShader(ShaderId id, Shader* shader) {
	shaders[id] = shader;
}

void ResourceManager::StoreShader(Shader* shader) {
	StoreShader(shader->GetId(), shader);
}

void ResourceManager::ReloadShaders() const
{
	cout << endl;
	cout << "=============================" << endl;
	cout << "Reloading Shaders" << endl;
	cout << "=============================" << endl;
	cout << endl;

	for (auto& shader : shaders)
	{
		shader.second->Reload();
	}
}

ResourceManager* Migine::GetResourceManager() {
	return ResourceManager::GetInstance();
}

template<> void ResourceManager::LoadMesh<MeshId::box>() {
	Mesh* mesh = new Mesh(MeshId::box);
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	StoreMesh(mesh);
}

template<> void ResourceManager::LoadMesh<MeshId::boxWireframe>() {
	Mesh* mesh = new Mesh(MeshId::boxWireframe);
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
	StoreMesh(mesh);
}

template<> void ResourceManager::LoadMesh<MeshId::plane>() {
	Mesh* mesh = new Mesh(MeshId::plane);
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
	StoreMesh(mesh);
}

template<> void ResourceManager::LoadMesh<MeshId::sphere>() {
	Mesh* mesh = new Mesh(MeshId::sphere);
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	StoreMesh(mesh);
}

template<> void ResourceManager::LoadShader<ShaderId::vertexColor>() {
	Shader* shader = new Shader(ShaderId::vertexColor);
	shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(RESOURCE_PATH::SHADERS + "VertexColor.FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	StoreShader(shader);
}

template<> void ResourceManager::LoadShader<ShaderId::vertexNormal>() {
	Shader* shader = new Shader(ShaderId::vertexNormal);
	shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(RESOURCE_PATH::SHADERS + "Normals.FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	StoreShader(shader);
}

template<> void ResourceManager::LoadShader<ShaderId::color>() {
	Shader* shader = new Shader(ShaderId::color);
	shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(RESOURCE_PATH::SHADERS + "Color.FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	StoreShader(shader);
}

template<> void ResourceManager::LoadShader<ShaderId::simple>() {
	Shader* shader = new Shader(ShaderId::simple);
	shader->AddShader(RESOURCE_PATH::SHADERS + "MVP.Texture.VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(RESOURCE_PATH::SHADERS + "Default.FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	StoreShader(shader);
}

template<> void ResourceManager::LoadShader<ShaderId::lab8>() {
	Shader* shader = new Shader(ShaderId::lab8);
	shader->AddShader("Source/Laboratoare/Laborator8/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	shader->AddShader("Source/Laboratoare/Laborator8/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	StoreShader(shader);
}