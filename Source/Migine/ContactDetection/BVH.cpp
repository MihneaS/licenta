#include "BVH.h"
#include <Migine/GameObjects/GameObject.h>

#include <Core/GPU/Mesh.h>

#include <queue>
#include <vector>
#include <cassert>
#include <iostream>

using namespace Migine;

using std::priority_queue;
using std::vector;
using std::cout;
using std::swap;
using std::tuple;
using std::make_tuple;

using glm::vec3;

using EngineComponents::Camera;

//BVH::Node::Node(GameObject* boundedObject, AABB* boundingVolume, Node* parent) :
//	boundingVolume(*boundingVolume), parent(parent), boundedObject(boundedObject) {
//}
BVH::Node::Node(GameObject* boundedObject, AABB* boundingVolume, Node* parent) :
	parent(parent), boundingVolume(*boundingVolume){
	//children[0] = nullptr;
	children[1] = nullptr;
	this->boundedObject = boundedObject;
}

BVH::Node::Node(Node* child0, Node* child1, Node* parent) :
	parent(parent), boundingVolume(&child0->boundingVolume, &child1->boundingVolume){
	children[0] = child0;
	children[1] = child1;
}

bool BVH::Node::IsLeaf() {
	return children[1] == nullptr;
}

int BVH::Node::GetIndexInParent() {
	assert(parent != nullptr);
	return (parent->children[1] == this) ? 1 : 0;
}

int BVH::Node::GetIndexOfChild(Node* child) {
	int index = (children[1] == child) ? 1 : 0;
	assert(children[index] == child);
	return index;
}

int BVH::Node::GetIndexOfBrother(Node* child) {
	return !GetIndexOfChild(child);
}

BVH::Node* BVH::Node::GetBrother(Node* child)
{
	return children[GetIndexOfBrother(child)];
}

void BVH::Node::RepalceChild(Node* oldChild, Node* newChild) {
	children[GetIndexOfChild(oldChild)] = newChild;
}

void BVH::Node::Refit() {
	boundingVolume.Resize(&children[0]->boundingVolume, &children[1]->boundingVolume);
}

BVH::~BVH() {
	if (tree_root) {
		DeleteTree(tree_root);
	}
}

void BVH::Insert(GameObject* gameObject) {
	//Insert(tree_root, gameObject);
	CacheContactsAndInsert(gameObject);
}

BVH::EnlargedVolumeGreater::EnlargedVolumeGreater(const AABB* addedVolume) :
	addedVolume(addedVolume) {
}

float BVH::EnlargedVolumeGreater::GetEnlargedVolume(const Node* toEnlarge) {
	auto it = cache.find(toEnlarge);
	if (it != cache.end()) {
		return it->second;
	}
	AABB tmp(addedVolume, &toEnlarge->boundingVolume);
	float volume = tmp.GetVolume();
	cache[toEnlarge] = volume;
	return volume;
}

bool BVH::EnlargedVolumeGreater::operator()(const Node* lhs, const Node* rhs) {
	// lhs < rhs is true => rhs is more important
	// lhs > rhs is true => lhs is more important
	float lhsVolume = lhs->boundingVolume.GetVolume();
	float rhsVolume = rhs->boundingVolume.GetVolume();
	float lhsEnlargedVolume = GetEnlargedVolume(lhs);
	float rhsEnlargedVolume = GetEnlargedVolume(rhs);

	//if (lhsVolume == lhsEnlargedVolume && rhsVolume != rhsEnlargedVolume) {
	//	return  false;
	//} else if (lhsVolume != lhsEnlargedVolume && rhsVolume == rhsEnlargedVolume) {
	//	return true;
	//} else {
	//	return lhsEnlargedVolume > rhsEnlargedVolume;
	//}

	bool lhsVolumeDidntEnlarge = lhsVolume == lhsEnlargedVolume;
	bool rhsVolumeDidntEnlarge = rhsVolume == rhsEnlargedVolume;
	return lhsVolumeDidntEnlarge == rhsVolumeDidntEnlarge && (lhsEnlargedVolume > rhsEnlargedVolume) ||
	       !lhsVolumeDidntEnlarge && rhsVolumeDidntEnlarge;
}

void BVH::Insert(Node* root, GameObject* gameObject, AABB* boundingVolume) {
	//AABB aabbForGameObj(gameObject);
	bool deleteBoundingVolume = false;
	if (boundingVolume == nullptr) {
		boundingVolume = new AABB(gameObject);
		deleteBoundingVolume = true;
	}
	Node* newLeafNode = new Node(gameObject, boundingVolume, nullptr);
	gameObject->bvhNode = newLeafNode;
	if (!tree_root) {
		tree_root = newLeafNode;
	}
	else {
		//EnlargedVolumeGreater greater(&aabbForGameObj);
		//priority_queue<Node*, vector<Node*>, EnlargedVolumeGreater> pq(greater);
		NodeGreater greater(boundingVolume);
		priority_queue<Node*, vector<Node*>, NodeGreater> pq(greater);
		while (!root->IsLeaf()) {
			/* 
			* aici godot (care se inspira din bullet) selecteaza pur si simplu "copilul mai apropiat",
			* definind distanta ca suma distantelor pe ox, oy si oz dintre centrele volumelor.
			* presupun ca acest mod e mai util pt fizica, pt rezultat real time.
			* 
			* Erin Catto propune formarea arborului astfel incat suma volumelor nodurilor interioare sa fie minima.
			* presupun ca acest mod e mai util pentru ray tracing, pentru cand merita sa consumi mai mult timp sa faci un arbore mai bun
			*/
			pq.push(root->children[0]);
			pq.push(root->children[1]);
			root = pq.top();
			pq.pop();
		}
		Node* newInternNode = new Node(root, newLeafNode, root->parent);
		newInternNode->children[0]->parent = newInternNode;
		newInternNode->children[1]->parent = newInternNode;
		if (newInternNode->parent == nullptr) { // TODO inlocuieste 
			tree_root = newInternNode;
		} else {
			newInternNode->parent->RepalceChild(root, newInternNode);

			root = root->parent;
			do {
				root->boundingVolume.EnlargeBy(boundingVolume);
				root = root->parent;
			} while (root);
		}
		/*
		* TODO optimize upwards
		*/
		//Print(cout);
	}
	if (deleteBoundingVolume) {
		delete boundingVolume;
	}
}

void BVH::Remove(GameObject* gameObject) {
	RemoveLeaf(gameObject->bvhNode);
	gameObject->bvhNode = nullptr;
	EraseGameObjectFromAllContacts(gameObject);
}

void BVH::RemoveLeaf(Node* leaf) {
	if (leaf == tree_root) {
		tree_root = nullptr;
	} else {
		Node* grandParent = leaf->parent->parent;
		Node* brother = leaf->parent->GetBrother(leaf);
		if (!grandParent) {
			tree_root = brother;
			tree_root->parent = nullptr;
		} else {

			grandParent->RepalceChild(leaf->parent, brother);
			brother->parent = grandParent;
			do {
				grandParent->Refit();
				grandParent = grandParent->parent;
			} while (grandParent);
			/*
			* Maybe optimize upwards?
			*/
		}
		delete leaf->parent;
	}
	delete leaf;
}


void BVH::DeleteTree(Node* root) {
	if (!root->IsLeaf()) {
		DeleteTree(root->children[0]);
		DeleteTree(root->children[1]);
	}
	delete root;
}

void BVH::CacheContact(GameObject* obj0, GameObject* obj1) {
	if (obj0 < obj1) {
		contactsCache.insert(make_tuple(obj0, obj1));
	} else {
		contactsCache.insert(make_tuple(obj1, obj0));
	}
	contactsGraph[obj0].insert(obj1);
	contactsGraph[obj1].insert(obj0);
}

void BVH::RemoveContact(GameObject* obj0, GameObject* obj1) {
	if (obj0 < obj1) {
		contactsCache.erase(make_tuple(obj0, obj1));
	} else {
		contactsCache.erase(make_tuple(obj1, obj0));
	}
	contactsGraph[obj0].erase(obj1);
	contactsGraph[obj1].erase(obj0);
}

void BVH::RenderAll(Camera* camera) {
	RenderAllRecursive(camera, tree_root);
}

void BVH::Print(std::ostream& outStream) {
	static int i = 1;
	outStream << i++ << "\n";
	PrintRecursive(outStream, tree_root, 0);
	outStream << "\n";
}

void BVH::Update(GameObject* gameObject) {
	// TODO foloseste un volum mai strans si unul mai mare, daca volumul mai stramt nu iese din ala mare nu modifica
	Remove(gameObject);
	Insert(gameObject);

}

#include <queue>;
using std::queue;

void BVH::CacheContacts(GameObject* gameObject) {
	if (tree_root == nullptr) {
		return;
	}
	AABB aabbForGameObj(gameObject);
	queue<Node*> q;
	q.push(tree_root);
	do {
		Node* n = q.front();
		q.pop();
		if (n->boundingVolume.DoesIntersect(&aabbForGameObj)) {
			if (n->IsLeaf()) {
				CacheContact(gameObject, n->boundedObject);
			} else {
				q.push(n->children[0]);
				q.push(n->children[1]);
			}
		}
	} while (!q.empty());
}

void BVH::CacheContactsAndInsert(GameObject* gameObject) {
	Node* bestInsertionPlace = tree_root;
	AABB aabbForGameObj(gameObject);
	if (tree_root != nullptr) {
		NodeGreater greater(&aabbForGameObj);
		queue<Node*> q;
		q.push(tree_root);
		do {
			Node* node = q.front();
			q.pop();
			if (node->boundingVolume.DoesIntersect(&aabbForGameObj)) {
				if (greater(node, bestInsertionPlace)) {
					bestInsertionPlace = node;
				}
				if (node->IsLeaf()) {
					CacheContact(gameObject, node->boundedObject);
				} else {
					q.push(node->children[0]);
					q.push(node->children[1]);
				}
			}
		} while (!q.empty());
	}
	Insert(bestInsertionPlace, gameObject, &aabbForGameObj);
}

void BVH::EraseGameObjectFromAllContacts(GameObject* gameObject) {
	auto iteratorContacts = contactsGraph.find(gameObject);
	if (iteratorContacts != contactsGraph.end()) {
		for (GameObject* otherGameObject : contactsGraph.at(gameObject)) {
			contactsGraph.at(otherGameObject).erase(gameObject);
			if (gameObject < otherGameObject) {
				contactsCache.erase(make_tuple(gameObject, otherGameObject));
			} else {
				contactsCache.erase(make_tuple(otherGameObject, gameObject));
			}
		}
		contactsGraph.erase(gameObject);
	}
}

size_t Migine::BVH::GetContactCount() {
	return contactsCache.size();
}

void BVH::RenderAllRecursive(Camera* camera, Node* root) {
	if (root == nullptr) {
		return;
	}
	auto it = rws4aabbs.find(root);
	if (it == rws4aabbs.end()) {
		rws4aabbs.emplace(root, &root->boundingVolume);
	}
	rws4aabbs.at(root).Render(camera);
	if (!root->IsLeaf()) {
		RenderAllRecursive(camera, root->children[0]);
		RenderAllRecursive(camera, root->children[1]);
	}
}

void BVH::PrintRecursive(std::ostream& outStream, Node* root, int level) {
	for (int i = 0; i < level; i++) {
		outStream << "|\t";
	}
	vec3 pos = root->boundingVolume.GetCenter();
	//outStream << meshIdToIdNames[root->boundedObject->mesh->GetId()] << " " << pos;
	outStream << pos;
	if (!root->IsLeaf()) {
		outStream << " {\n";
		level++;
		PrintRecursive(outStream, root->children[0], level);
		PrintRecursive(outStream, root->children[1], level);
		for (int i = 0; i < level-1; i++) {
			outStream << "|\t";
		}
		outStream << "}\n";
	}
	else {
		outStream << " Leaf\n";
	}
}

BVH::ManhattanDistanceGreater::ManhattanDistanceGreater(const AABB* addedVolume) :
	addedVolume(addedVolume) {
}

float Migine::BVH::ManhattanDistanceGreater::GetDistance(const Node* other) {
	auto it = cache.find(other);
	if (it != cache.end()) {
		return it->second;
	}
	vec3 addedVolumeCenter = addedVolume->GetCenter();
	vec3 otherCenter = other->boundingVolume.GetCenter();
	float distance = abs(addedVolumeCenter.x - otherCenter.x) +
	                 abs(addedVolumeCenter.y - otherCenter.y) +
	                 abs(addedVolumeCenter.z - otherCenter.z);
	cache[other] = distance;
	return distance;
}

bool BVH::ManhattanDistanceGreater::operator()(const Node* lhs, const Node* rhs) {
	return GetDistance(lhs) > GetDistance(rhs);
}
