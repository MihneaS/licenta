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
	Insert(tree_root, gameObject);
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

void BVH::Insert(Node* root, GameObject* gameObject) {
	AABB aabbForGameObj(gameObject);
	Node* newLeafNode = new Node(gameObject, &aabbForGameObj, nullptr);
	gameObject->bvhNode = newLeafNode;
	if (!tree_root) {
		tree_root = newLeafNode;
	}
	else {
		EnlargedVolumeGreater greater(&aabbForGameObj);
		priority_queue<Node*, vector<Node*>, EnlargedVolumeGreater> pq(greater);
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
				root->boundingVolume.EnlargeBy(&aabbForGameObj);
				root = root->parent;
			} while (root);
		}
		/*
		* TODO optimize upwards
		*/
		//Print(cout);
	}
}

void BVH::Remove(GameObject* gameObject) {
	RemoveLeaf(gameObject->bvhNode);
	gameObject->bvhNode = nullptr;
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

void BVH::RenderAllRecursive(Camera* camera, Node* root) {
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
