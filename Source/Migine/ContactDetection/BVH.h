#pragma once

//#include <Migine/GameObjects/GameObject.h>
#include <Migine/ContactDetection/AABB.h>
#include <Migine/ContactDetection/RenderedWraperForAABB.h>
#include <Migine/utils.h>
#include <Migine/define.h>

#include <Component/SimpleScene.h>
#include <Migine/Camera.h>

#include <unordered_map>
#include <unordered_set>
#include <tuple>


namespace Migine {
	class BaseCollider;

	class BVH {
		friend class BaseCollider;

		class Node {
		public:
			// memory managed by the BVH managing this Node
			Node* parent = nullptr;
			AABB boundingVolume;
			union {
				// memory managed by the BVH managing this Node
				Node* children[2] = {nullptr, nullptr};
				// memory managed by someone else
				BaseCollider* boundedObject;
			};

			Node(BaseCollider* boundedObject, AABB* boundingVolume, Node* parent);
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

		//typedef ManhattanDistanceGreater NodeGreater; // easy to switch Greater
		typedef EnlargedVolumeGreater NodeGreater; // easy to switch Greater

		// memory managed by this
		Node* tree_root = nullptr; // TODO change name to bvhRoot
		std::unordered_map<Node*, RenderedWraperForAABB> rws4aabbs;
		std::unordered_map<BaseCollider*, std::unordered_set<BaseCollider*> > contactsGraph;
		std::unordered_set<std::tuple<BaseCollider*, BaseCollider*>, Migine::TupleHasher<BaseCollider*, BaseCollider*> > contactsCache;
#ifdef DEBUGGING
	public:
		static int insertionCount;
		static int AABBIntersectionOperationsCount;
#endif // DEBUGGING

	public:
		BVH() = default;
		~BVH();

		void Insert(BaseCollider* collider);
		void Remove(BaseCollider* collider);
		void RenderAll(Camera* camera);
		void Print(std::ostream& outStream);
		void Update(BaseCollider* collider); // TODO find better name
		void CacheContacts(BaseCollider* collider);
		void CacheContactsAndInsert(BaseCollider* collider);
		void EraseFromAllContacts(BaseCollider* collider);
		size_t GetContactCount();

	private:
		void Insert(Node* root, BaseCollider* collider, AABB* boundingVolume = nullptr);
		void RemoveLeaf(Node* leaf);
		void DeleteTree(Node* root);
		void CacheContact(BaseCollider* collider0, BaseCollider* collider1);
		void RemoveContact(BaseCollider* colldier0, BaseCollider* collider1);

	private:
		void RenderAllRecursive(Camera* camera, Node* root);
		void PrintRecursive(std::ostream& outStream, Node* root, int level);
	};
}
