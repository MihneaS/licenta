#pragma once

#include <Migine/GameObjects/GameObject.h>
#include <Migine/BroadContactDetection/AABB.h>
#include <Migine/BroadContactDetection/RenderedWraperForAABB.h>

#include <Component/SimpleScene.h>
#include <Component/Camera/Camera.h>

#include <unordered_map>


namespace Migine {
	class BVH {
		class Node {
		public:
			static Node null;
			Node* parent = nullptr;
			AABB boundingVolume;
			union {
				Node* children[2] = {nullptr, nullptr};
				//struct {
				//GameObject* boundedObject[2];// = { nullptr, nullptr };
					//void* ceva = nullptr;
				//};
				GameObject* boundedObject;
			};

			//Node(GameObject* gameObject, Node* parent);
			Node(GameObject* gameObject, AABB* boundingVolume, Node* parent);
			Node(Node* child0, Node* child1, Node* parent);

			bool IsLeaf();
			int GetIndexInParent();
			int GetIndexOfChild(Node* child);
			void RepalceChild(Node* oldChild, Node* newChild);
			friend class NodeLess;
		};
		//static Node nullNode;

		class EnlargedVolumeGreater {
			const AABB* addedVolume;
			std::unordered_map<const Node*, float> cache;


		public:
			EnlargedVolumeGreater(const AABB* addedVolume);
		private:
			float GetEnlargedVolume(const Node* toEnlarge);
		public:
			bool operator() (const Node* lhs, const Node* rhs);
		};

		Node* tree_root = nullptr; // TODO change name to bvhRoot
		std::unordered_map<Node*, RenderedWraperForAABB> rws4aabbs;

	public:
		BVH() = default;
		~BVH();

		void Insert(GameObject* gameObject);
		void Insert(Node* root, GameObject* gameObject);
		void DeleteTree(Node* root);
		void RenderAll(EngineComponents::Camera* camera);
		void Print(std::ostream& outStream);

	private:
		void RenderAllRecursive(EngineComponents::Camera* camera, Node* root);
		void PrintRecursive(std::ostream& outStream, Node* root, int level);
	};
}
