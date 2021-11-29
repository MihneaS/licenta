#pragma once

//#include <Migine/GameObjects/GameObject.h>
#include <Migine/BroadContactDetection/AABB.h>
#include <Migine/BroadContactDetection/RenderedWraperForAABB.h>
#include <Migine/utils.h>

#include <Component/SimpleScene.h>
#include <Component/Camera/Camera.h>

#include <unordered_map>
#include <unordered_set>
#include <tuple>


namespace Migine {
	class GameObject;

	class BVH {
		friend class GameObject;

		class Node {
		public:
			// memory managed by the BVH managing this Node
			Node* parent = nullptr;
			AABB boundingVolume;
			union {
				// memory managed by the BVH managing this Node
				Node* children[2] = {nullptr, nullptr};
				// memory managed by someone else
				GameObject* boundedObject;
			};

			//Node(GameObject* gameObject, Node* parent);
			Node(GameObject* gameObject, AABB* boundingVolume, Node* parent);
			Node(Node* child0, Node* child1, Node* parent);

			bool IsLeaf();
			int GetIndexInParent();
			int GetIndexOfChild(Node* child);
			int GetIndexOfBrother(Node* child);
			Node* GetBrother(Node* child);
			void RepalceChild(Node* oldChild, Node* newChild);
			void Refit(); // TODO find better verb
			friend class NodeLess;
		};
		//static Node nullNode;

		class EnlargedVolumeGreater {
			// memory managed by someone else
			const AABB* addedVolume;
			std::unordered_map<const Node*, float> cache;
		public:
			EnlargedVolumeGreater(const AABB* addedVolume);
		private:
			float GetEnlargedVolume(const Node* toEnlarge);
		public:
			bool operator() (const Node* lhs, const Node* rhs);
		};
		class ManhattanDistanceGreater {
			// memory managed by someone else
			const AABB* addedVolume;
			std::unordered_map<const Node*, float> cache;
		public:
			ManhattanDistanceGreater(const AABB* addedVolume);
		private:
			float GetDistance(const Node* to);
		public:
			bool operator() (const Node* lhs, const Node* rhs);
		};
		template <class T>
		struct TupleOrderlessEqualTo {
			bool operator()(std::tuple<T, T>& lhs, std::tuple<T, T>& rhs) const {
				return lhs.first == rhs.first && lhs.second == rhs.second ||
				       lhs.first == rhs.second && lhs.second == rhs.first;
			}
		};

		typedef ManhattanDistanceGreater NodeGreater; // easy to switch Greater
		// memory managed by this
		Node* tree_root = nullptr; // TODO change name to bvhRoot
		std::unordered_map<Node*, RenderedWraperForAABB> rws4aabbs;
		std::unordered_map<GameObject*, std::unordered_set<GameObject*> > contactsGraph;
		std::unordered_set<std::tuple<GameObject*, GameObject*>, Migine::TupleHasher<GameObject*, GameObject*> > contactsCache;

	public:
		BVH() = default;
		~BVH();

		void Insert(GameObject* gameObject);
		void Remove(GameObject* gameObject);
		void RenderAll(EngineComponents::Camera* camera);
		void Print(std::ostream& outStream);
		void Update(GameObject* gameObject); // TODO find better name
		void CacheContacts(GameObject* gameObject);
		void CacheContactsAndInsert(GameObject* gameObject);
		void EraseGameObjectFromAllContacts(GameObject* gameobject);
		size_t GetContactCount();

	private:
		void Insert(Node* root, GameObject* gameObject, AABB* boundingVolume = nullptr);
		void RemoveLeaf(Node* leaf);
		void DeleteTree(Node* root);
		void CacheContact(GameObject* obj0, GameObject* obj1);
		void RemoveContact(GameObject* obj0, GameObject* obj1);

	private:
		void RenderAllRecursive(EngineComponents::Camera* camera, Node* root);
		void PrintRecursive(std::ostream& outStream, Node* root, int level);
	};
}
