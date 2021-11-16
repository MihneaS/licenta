#pragma once

#include <Migine/GameObjects/GameObject.h>

namespace Migine {
	class Sphere : public GameObject {
	public:
		Sphere(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());
	};
}