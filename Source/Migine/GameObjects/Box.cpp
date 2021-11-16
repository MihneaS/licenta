#include "Box.h"

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


Box::Box(vec3 position, vec3 scale, quat rotation) :
		GameObject(GetMesh<MeshId::box>(), new SimpleRenderer(GetShader<ShaderId::vertexNormal>(), this),
			position, scale, rotation) {
}