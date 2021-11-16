#include "Sphere.h"

#include <vector>
#include <algorithm>
#include <Migine/Renderers/SimpleRenderer.h>
#include <Migine/ResourceManager.h>

using namespace Migine;
using glm::vec3;
using glm::vec4;
using glm::quat;
using EngineComponents::Transform;
using std::vector;
using std::min;
using std::max;


Sphere::Sphere(vec3 position, vec3 scale, quat rotation) :
	GameObject(GetMesh<MeshId::sphere>(), new SimpleRenderer(GetShader<ShaderId::vertexNormal>(), this),
		position, scale, rotation) {
}