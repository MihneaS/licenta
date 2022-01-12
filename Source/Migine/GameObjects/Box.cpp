#include "Box.h"

#include <vector>
#include <algorithm>
#include <Migine/Renderers/SimpleRenderer.h>
#include <Migine/ResourceManager.h>
#include <Migine/ContactDetection/BoxCollider.h>

using namespace Migine;
using glm::vec3;
using glm::vec4;
using glm::quat;
using std::vector;
using std::min;
using std::max;


Box::Box(vec3 position, vec3 scale, quat rotation) :
		Box(position, scale, rotation, GetMesh<MeshId::box>()) {
}

Box::Box(vec3 position, vec3 scale, quat rotation, Mesh* mesh) :
	GameObject(mesh, new SimpleRenderer(GetShader<ShaderId::vertexNormal>(), this), new BoxCollider(&this->transform, mesh),
		position, scale, rotation) {
}
