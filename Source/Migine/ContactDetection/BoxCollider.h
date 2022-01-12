#pragma once

#include <Migine/Transform.h>
#include <Migine/ContactDetection/BaseCollider.h>
#include <Core/GPU/Mesh.h>

#include <unordered_map>
#include <vector>

namespace Migine {
	class BoxCollider: public BaseCollider {
	private:
		static inline std::unordered_map<MeshId, std::vector<glm::vec3> > cache;
		std::vector<glm::vec3> localCorners; // corners positions in local space.
		//TODO should I use std::array?
	
	public:
		BoxCollider(Transform* transform, Mesh* mesh);

		Collision CheckCollision(BaseCollider* other) override; // double dispatch T.T I hate it
		Collision CheckCollision(BoxCollider* other) override;
		Collision CheckCollision(SphereCollider* other) override;
		std::tuple<glm::vec3, glm::vec3> ProvideAABBParameters() override;

	private:
		void ConstructLocalCorners(Mesh* mesh);
	};
}