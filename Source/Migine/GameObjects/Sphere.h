#pragma once

#include <Migine/GameObjects/GameObject.h>

namespace Migine {
	class Sphere : public GameObject {
	public:
		Sphere(glm::vec3 position = { 0,0,0 }, float diameter = 1, glm::quat rotation = glm::quat());
	private:
		Sphere(glm::vec3 position, float diameter, glm::quat rotation, Mesh* mesh);
	};
}