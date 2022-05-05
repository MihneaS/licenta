#pragma once

#include <migine/contact_detection/AABB.h>
#include <migine/utils.h>
#include <migine/Camera.h>
#include <migine/define.h>

#include <unordered_map>
#include <unordered_set>
#include <array>
#include <memory>

#include <gsl/gsl>

namespace migine {
	class Collider_base;

	class BVH {
		friend class Collider_base; // TODO e necesar?
		friend class AABB;

		class Node {
			friend class Node_less;

		public:
			Node(Collider_base* bounded_object, AABB bounding_volume, Node* parent);
			Node(std::array<std::unique_ptr<Node>, 2> children, Node* parent);
			~Node();

			bool is_leaf() const;
			int get_index_in_parent() const;
			int get_index_of_child(const Node* child) const;
			int get_index_of_brother(const Node* child) const;
			Node* get_brother(const Node* child) const;
			void repalce_child(const Node* old_child, std::unique_ptr<Node> new_child);
			void refit(); // TODO find better verb
			void refit_all_upwards();

			// memory managed by the BVH managing this Node
			AABB bounding_volume;
			Node* parent = nullptr;
			// union is smaller then variant
			using children_array_t = std::array<std::unique_ptr<Node>, 2>;
			union {
				children_array_t children;
				gsl::not_null<Collider_base*> bounded_object;
			};
		};
		//static Node nullNode;

		class Enlarged_volume_greater {
		public:
			Enlarged_volume_greater(AABB added_volume);
			bool operator() (const Node* lhs, const Node* rhs);

		private:
			float get_enlarged_volume(gsl::not_null<const Node*> to_enlarge);

			AABB added_volume;
			std::unordered_map<gsl::not_null<const Node*>, float> cache;
		};
		class Manhattan_distance_greater {
		public:
			Manhattan_distance_greater(AABB added_volume);
			bool operator() (const Node* lhs, const Node* rhs);

		private:
			float get_distance(gsl::not_null<const Node*> to);

			AABB added_volume;
			std::unordered_map<gsl::not_null<const Node*>, float> cache;
		};

		//typedef ManhattanDistanceGreater NodeGreater; // easy to switch Greater
		typedef Enlarged_volume_greater Node_greater; // easy to switch Greater
		typedef std::unordered_set<std::tuple<gsl::not_null<Collider_base*>, gsl::not_null<Collider_base*>>, migine::Tuple_hasher<Collider_base*, Collider_base*>> contacts_cache_t;

	public:
		BVH() = default;

		void insert(gsl::not_null<Collider_base*> collider);
		void remove(gsl::not_null<Collider_base*> collider);
		void print(std::ostream& out_stream) const;
		void update(gsl::not_null<Collider_base*> collider); // TODO find better name
		void cache_contacts(gsl::not_null<Collider_base*> collider);
		void cache_contacts_and_insert(gsl::not_null<Collider_base*> collider);
		void erase_from_all_contacts(gsl::not_null<Collider_base*> collider);
		const contacts_cache_t& get_contacts() const;
		size_t get_contact_count() const;
		void clean_dirty_nodes();

#ifdef DEBUGGING
		void render_all(const Camera& camera) const;

		static int insertion_count;
		static int aabb_intersection_operations_count;
#endif // DEBUGGING

	private:
		void insert(Node* root, gsl::not_null<Collider_base*> collider, AABB bounding_volume);
		void insert(Node* root, gsl::not_null<Collider_base*> collider);
		void remove_leaf(gsl::not_null<Node*> leaf);
		void cache_contact(gsl::not_null<Collider_base*> collider0, gsl::not_null<Collider_base*> collider1);
		void remove_contact(gsl::not_null<Collider_base*> colldier0, gsl::not_null<Collider_base*> collider1);
		void print_recursive(std::ostream& out_stream, Node* root, int level) const;
		void mark_dirty_node(gsl::not_null<Node*> node);

		bool less_for_unique_cache_entry(gsl::not_null<const Collider_base*> lhs, gsl::not_null<const Collider_base*> rhs) const;

		std::unique_ptr<Node> bvh_root;
		std::unordered_map<gsl::not_null<Collider_base*>, std::unordered_set<gsl::not_null<Collider_base*>>> contacts_graph;
		contacts_cache_t contacts_cache;
		std::unordered_set<gsl::not_null<Node*>> dirty_nodes;

		int get_graph_size() const;

#ifdef DEBUGGING
		void render_all_recursive(const Camera& camera, Node* root) const;
#endif
	};
}
