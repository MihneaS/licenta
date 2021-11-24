#pragma once

#include <Core/Engine.h>
#include <string>

namespace Migine {
	glm::quat EulerAnglesToQuat(glm::vec3 eulerAngles);
	glm::quat EulerAnglesDegToQuat(glm::vec3 eulerAngles);
	void PrintFps(float deltaTime);
	int ContinousPrint(std::string s, int index);
}