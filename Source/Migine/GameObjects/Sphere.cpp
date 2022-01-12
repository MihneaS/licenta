#include "Sphere.h"

#include <vector>
#include <algorithm>
#include <Migine/Renderers/SimpleRenderer.h>
#include <Migine/ResourceManager.h>
#include <Migine/ContactDetection/SphereCollider.h>

using namespace Migine;
using glm::vec3;
using glm::vec4;
using glm::quat;
using EngineComponents::Transform;
using std::vector;
using std::min;
using std::max;


Sphere::Sphere(vec3 position, float diameter, quat rotation) :
	Sphere(position, diameter, rotation, GetMesh<MeshId::sphere>()) {
}

Sphere::Sphere(glm::vec3 position, float diameter, glm::quat rotation, Mesh* mesh) :
	GameObject(mesh, new SimpleRenderer(GetShader<ShaderId::vertexNormal>(), this), new SphereCollider(&this->transform, mesh),
	position, {diameter, diameter, diameter}, rotation) {
}
