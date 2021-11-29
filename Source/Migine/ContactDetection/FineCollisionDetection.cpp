#include "FineCollisionDetection.h"

using namespace Migine;

Collision FineCollisionDetector::operator()(BoxCollider* box0, BoxCollider* box1) {
	assert(false); // neimplementat
	return Collision();
}

Collision FineCollisionDetector::operator()(BoxCollider* box, SphereCollider* sphere) {
	assert(false); // neimplementat
	return Collision();
}

Collision FineCollisionDetector::operator()(SphereCollider* sphere, BoxCollider* box) {
	return operator()(box, sphere);
}

Collision FineCollisionDetector::operator()(SphereCollider* sphere0, SphereCollider* sphere1) {
	assert(false); // neimplementat
	return Collision();
}

Collision Migine::FineCollisionCheck(BaseCollider* collider0, BaseCollider* collider1) {
	return Collision();
}