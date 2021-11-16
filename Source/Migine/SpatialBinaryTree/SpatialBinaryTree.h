#pragma once
#include <Migine/GameObjects/GameObject.h>
#include <Component/SimpleScene.h>
/*
namespace Migine {
	class SpatialBinaryTree {
		class Node {
		public:
			Node* parent;
			AABB boundingVolume;
			union {
				Node* children[2] = {nullptr, nullptr};
				GameObject* gameObject;
			};

			Node(GameObject* gameObject, Node* parent);
			//Node(Node* children[2], Node* parent);
			Node(Node* child0, Node* child1, Node* parent);

			bool isLeaf();
		};

		SimpleScene* scene;
		Node* tree_root = nullptr;

		SpatialBinaryTree(SimpleScene* scene);
		~SpatialBinaryTree();

		void insert(GameObject* gameObject);
		void insert(Node* root, GameObject* gameObject);
		void deleteTree(Node* root);


	};
}
*/