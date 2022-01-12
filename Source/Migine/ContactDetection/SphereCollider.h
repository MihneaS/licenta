#pragma once

#include <Migine/ContactDetection/BaseCollider.h>

#include <tuple>

namespace Migine {
	class SphereCollider: public BaseCollider {
	private:
		glm::vec3 center;
		float radius;

		static inline std::unordered_map<MeshId, std::tuple<glm::vec3, float>> cache;
	public:
		SphereCollider(Transform* transform, Mesh* mesh);

		Collision CheckCollision(BaseCollider* other) override; // double dispatch T.T I hate it
		Collision CheckCollision(BoxCollider* other) override;
		Collision CheckCollision(SphereCollider* other) override;

		virtual std::tuple<glm::vec3, glm::vec3> ProvideAABBParameters() override;

		float GetDiameter();
	private:
		void ComputeCenterAndRadius(Mesh* mesh);
	};
}