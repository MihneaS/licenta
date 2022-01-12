#pragma once

#include <Migine/GameObjects/GameObject.h>

namespace Migine {
	class Box : public GameObject {
	public:
		Box(glm::vec3 position = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::quat rotation = glm::quat());

	private:
		Box(glm::vec3 position, glm::vec3 scale, glm::quat rotation, Mesh* mesh);
	};
}