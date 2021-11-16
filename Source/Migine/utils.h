#pragma once

#include <Core/Engine.h>

namespace Migine {
	glm::quat EulerAnglesToQuat(glm::vec3 eulerAngles);
	glm::quat EulerAnglesDegToQuat(glm::vec3 eulerAngles);
}