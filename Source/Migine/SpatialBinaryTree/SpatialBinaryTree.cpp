#include "SpatialBinaryTree.h"

using namespace Migine;
/*
SpatialBinaryTree::Node::Node(GameObject* gameObject, Node* parent) :
	boundingVolume(gameObject->scene, gameObject), parent(parent) {
}

SpatialBinaryTree::Node::Node(Node* child0, Node* child1, Node* parent) :
	boundingVolume()
{
}

bool SpatialBinaryTree::Node::isLeaf()
{
	return children[1] == nullptr;
}

SpatialBinaryTree::SpatialBinaryTree(SimpleScene* scene) :
	scene(scene) {
}

SpatialBinaryTree::~SpatialBinaryTree() {
	if (tree_root) {
		deleteTree(tree_root);
	}
}

void SpatialBinaryTree::insert(GameObject* gameObject) {
	insert(tree_root, gameObject);
}

void SpatialBinaryTree::insert(Node* root, GameObject* gameObject) {
	if (!tree_root) {
		tree_root = new Node(gameObject, nullptr);
	}
	else {
		while (!root->isLeaf()) {
			//root = root->selectchild(gameObject)
		}
		Node* newNode = new Node(root, new Node(gameObject, nullptr), root->parent);
		newNode->children[0]->parent = newNode;
		newNode->children[1]->parent = newNode;
	}
}

void SpatialBinaryTree::deleteTree(Node* root) {
	if (!root->isLeaf()) {
		deleteTree(root->children[0]);
		deleteTree(root->children[1]);
	}
	delete root;
}
*/