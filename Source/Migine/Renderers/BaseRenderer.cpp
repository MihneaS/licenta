#pragma once

#include "BaseRenderer.h"
#include <Component/Transform/Transform.h>
#include <Migine/RenderedObject.h>

using namespace Migine;
using glm::vec3;
using glm::quat;

BaseRenderer::BaseRenderer(const Shader* shader, RenderedObject* renderedObject) :
	shader(shader), renderedObject(renderedObject) {
}