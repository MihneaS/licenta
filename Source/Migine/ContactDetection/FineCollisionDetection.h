#pragma once

#include <Migine/ContactDetection/BaseCollider.h>
#include <Migine/ContactDetection/BoxCollider.h>
#include <Migine/ContactDetection/SphereCollider.h>
#include <Migine/ContactDetection/Collision.h>

namespace Migine {
	class FineCollisionDetector {
	public:
		Collision operator()(BoxCollider* box0, BoxCollider* box1);
		Collision operator()(BoxCollider* box, SphereCollider* sphere);
		Collision operator()(SphereCollider* sphere, BoxCollider* box);
		Collision operator()(SphereCollider* sphere0, SphereCollider* sphere1);
	};

	Collision FineCollisionCheck(BaseCollider* collider0, BaseCollider* collider1);
}