#include "BVH.h"

#include <migine/contact_detection/Collider_base.h>
#include <migine/define.h>

#include <vector>
#include <queue>
#include <stack>

#include <iostream>

using glm::vec3;

using std::vector;
using std::priority_queue;
using std::queue;
using std::stack;
using std::make_tuple;
using std::array;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::array;
using std::unordered_set;
using std::swap;

using std::cout;

using gsl::not_null;

namespace migine {
#ifdef DEBUGGING
	int BVH::insertion_count = 0;
	int BVH::aabb_intersection_operations_count = 0;
#endif // DEBUGGING

	BVH::Node::Node(Collider_base* bounded_object, AABB bounding_volume, Node* parent) :
		parent(parent), bounding_volume(bounding_volume) {
		children[1] = nullptr;
		this->bounded_object = bounded_object;
	}

	BVH::Node::Node(array<unique_ptr<Node>, 2> children, Node* parent) :
		children(move(children)), parent(parent), bounding_volume(children[0]->bounding_volume, children[1]->bounding_volume) {
	}

	BVH::Node::~Node() {
		if (!is_leaf()) {
			children[0].reset();
			children[1].reset();
		}
	}

	bool BVH::Node::is_leaf() const {
		return children[1] == nullptr;
	}

	int BVH::Node::get_index_in_parent() const {
		assert(parent != nullptr);
		return (parent->children[1].get() == this) ? 1 : 0;
	}

	int BVH::Node::get_index_of_child(const Node* child) const {
		int index = (children[1].get() == child) ? 1 : 0;
		assert(children[index].get() == child);
		return index;
	}

	int BVH::Node::get_index_of_brother(const Node* child) const {
		return !get_index_of_child(child);
	}

	BVH::Node* BVH::Node::get_brother(const Node* child) const {
		return children[get_index_of_brother(child)].get();
	}

	void BVH::Node::repalce_child(const Node* old_child, unique_ptr<Node> new_child) {
		children[get_index_of_child(old_child)] = move(new_child);
	}

	void BVH::Node::refit() {
		bounding_volume.resize(children[0]->bounding_volume, children[1]->bounding_volume);
	}

	void BVH::Node::refit_all_upwards() {
		auto current = this;
		do {
			current->refit();
			current = current->parent;
		} while (current);
	}

	void BVH::insert(not_null<Collider_base*> collider) {
		cache_contacts_and_insert(collider);
#ifdef DEBUGGING
		BVH::insertion_count++;
#endif
	}

	BVH::Enlarged_volume_greater::Enlarged_volume_greater(AABB added_volume) :
		added_volume(added_volume) {
	}

	float BVH::Enlarged_volume_greater::get_enlarged_volume(not_null<const Node*> to_enlarge) {
		auto it = cache.find(to_enlarge);
		if (it != cache.end()) {
			return it->second;
		}
		AABB tmp(added_volume, to_enlarge->bounding_volume);
		float volume = tmp.get_volume();
		cache[to_enlarge] = volume;
		return volume;
	}

	bool BVH::Enlarged_volume_greater::operator()(const Node* lhs, const Node* rhs) {
		// lhs < rhs is true => rhs is more important
		// lhs > rhs is true => lhs is more important
		float lhs_volume = lhs->bounding_volume.get_volume();
		float rhs_volume = rhs->bounding_volume.get_volume();
		float lhs_enlarged_volume = get_enlarged_volume(lhs);
		float rhs_enlarged_volume = get_enlarged_volume(rhs);

		//if (lhs_volume == lhs_enlarged_volume && rhs_volume != rhs_enlarged_volume) {
		//	return  false;
		//} else if (lhs_volume != lhs_enlarged_volume && rhs_volume == rhs_enlarged_volume) {
		//	return true;
		//} else {
		//	return lhs_enlarged_volume > rhs_enlarged_volume;
		//}

		bool lhs_volume_didnt_enlarge = lhs_volume == lhs_enlarged_volume;
		bool rhs_volume_didnt_enlarge = rhs_volume == rhs_enlarged_volume;
		return lhs_volume_didnt_enlarge == rhs_volume_didnt_enlarge && (lhs_enlarged_volume > rhs_enlarged_volume) ||
			!lhs_volume_didnt_enlarge && rhs_volume_didnt_enlarge;
	}

	void BVH::insert(Node* root, not_null<Collider_base*> collider) {
		insert(root, collider, AABB(collider));
	}

	void BVH::insert(Node* root, not_null<Collider_base*> collider, AABB bounding_volume) {
		insert_by_volume(root, collider, bounding_volume);
	}

	void BVH::insert_old(Node* root, not_null<Collider_base*> collider, AABB bounding_volume) {
		unique_ptr<Node> new_leaf_node = make_unique<Node>(collider, bounding_volume, nullptr);
		collider->transform.bvh_node = new_leaf_node.get();
		if (!bvh_root) {
			bvh_root = move(new_leaf_node);
		} else {
			Node_greater greater(bounding_volume);
			priority_queue<Node*, vector<Node*>, Node_greater> pq(greater);
			while (!root->is_leaf()) {
				/*
				* aici godot (care se inspira din bullet) selecteaza pur si simplu "copilul mai apropiat",
				* definind distanta ca suma distantelor pe ox, oy si oz dintre centrele volumelor.
				* presupun ca acest mod e mai util pt fizica, pt rezultat real time.
				*
				* Erin Catto propune formarea arborului astfel incat suma volumelor nodurilor interioare sa fie minima.
				* presupun ca acest mod e mai util pentru ray tracing, pentru care merita sa consumi mai mult timp sa faci un arbore mai bun
				*/
				pq.push(root->children[0].get());
				pq.push(root->children[1].get());
				root = pq.top();
				pq.pop();
			}
			if (Node* parent = root->parent; !parent) {
				bvh_root = make_unique<Node>(BVH::Node::children_array_t{move(bvh_root), move(new_leaf_node)}, nullptr);
				bvh_root->children[0]->parent = bvh_root.get();
				bvh_root->children[1]->parent = bvh_root.get();
			} else {
				int root_idx = parent->get_index_of_child(root);
				unique_ptr<Node> new_intern_node = make_unique<Node>(BVH::Node::children_array_t{move(parent->children[root_idx]), move(new_leaf_node)}, root->parent);
				new_intern_node->children[0]->parent = new_intern_node.get();
				new_intern_node->children[1]->parent = new_intern_node.get();
				new_intern_node->parent->children[root_idx] = move(new_intern_node);
				root = root->parent;
				do {
					root->bounding_volume.enlarge_by(bounding_volume);
					root = root->parent;
				} while (root);
			}
		}
	}


	// inspired by godot that was inspired by bullet
	void BVH::insert_by_manhatten(Node* root, not_null<Collider_base*> collider, AABB bounding_volume) {
		unique_ptr<Node> new_leaf_node = make_unique<Node>(collider, bounding_volume, nullptr);
		collider->transform.bvh_node = new_leaf_node.get();
		if (!bvh_root) {
			bvh_root = move(new_leaf_node);
		} else {
			while (!root->is_leaf()) {
				float d0 = manhatten_distance(root->children[0]->bounding_volume.get_center(), bounding_volume.get_center());
				float d1 = manhatten_distance(root->children[1]->bounding_volume.get_center(), bounding_volume.get_center());
				root = d0 < d1 ? root->children[0].get() : root->children[1].get();
			}
			if (Node* parent = root->parent; !parent) {
				bvh_root = make_unique<Node>(BVH::Node::children_array_t{move(bvh_root), move(new_leaf_node)}, nullptr);
				bvh_root->children[0]->parent = bvh_root.get();
				bvh_root->children[1]->parent = bvh_root.get();
			} else {
				int root_idx = parent->get_index_of_child(root);
				unique_ptr<Node> new_intern_node = make_unique<Node>(BVH::Node::children_array_t{move(parent->children[root_idx]), move(new_leaf_node)}, root->parent);
				new_intern_node->children[0]->parent = new_intern_node.get();
				new_intern_node->children[1]->parent = new_intern_node.get();
				new_intern_node->parent->children[root_idx] = move(new_intern_node);
				root = root->parent;
				do {
					root->bounding_volume.enlarge_by(bounding_volume);
					root = root->parent;
				} while (root);
			}
		}
	}

	void BVH::insert_by_manhatten_mid_stop(Node* root, gsl::not_null<Collider_base*> collider, AABB bounding_volume) {
		unique_ptr<Node> new_leaf_node = make_unique<Node>(collider, bounding_volume, nullptr);
		collider->transform.bvh_node = new_leaf_node.get();
		if (!bvh_root) {
			bvh_root = move(new_leaf_node);
		} else {
			while (!root->is_leaf()) {
				float dr = manhatten_distance(root->bounding_volume.get_center(), bounding_volume.get_center());
				float d0 = manhatten_distance(root->children[0]->bounding_volume.get_center(), bounding_volume.get_center());
				float d1 = manhatten_distance(root->children[1]->bounding_volume.get_center(), bounding_volume.get_center());
				if (dr < d0 && dr < d1) {
					break;
				}
				root = d0 < d1 ? root->children[0].get() : root->children[1].get();
			}
			if (Node* parent = root->parent; !parent) {
				bvh_root = make_unique<Node>(BVH::Node::children_array_t{move(bvh_root), move(new_leaf_node)}, nullptr);
				bvh_root->children[0]->parent = bvh_root.get();
				bvh_root->children[1]->parent = bvh_root.get();
			} else {
				int root_idx = parent->get_index_of_child(root);
				unique_ptr<Node> new_intern_node = make_unique<Node>(BVH::Node::children_array_t{move(parent->children[root_idx]), move(new_leaf_node)}, root->parent);
				new_intern_node->children[0]->parent = new_intern_node.get();
				new_intern_node->children[1]->parent = new_intern_node.get();
				new_intern_node->parent->children[root_idx] = move(new_intern_node);
				root = root->parent;
				do {
					root->bounding_volume.enlarge_by(bounding_volume);
					root = root->parent;
				} while (root);
			}
		}
	}

	// inspired by box2d
	void BVH::insert_by_volume(Node* root, gsl::not_null<Collider_base*> collider, AABB bounding_volume) {
		unique_ptr<Node> new_leaf_node = make_unique<Node>(collider, bounding_volume, nullptr);
		collider->transform.bvh_node = new_leaf_node.get();
		if (!bvh_root) {
			bvh_root = move(new_leaf_node);
		} else {
			while (!root->is_leaf()) {
				float volume = root->bounding_volume.get_volume();

				AABB combined_AABB(root->bounding_volume, bounding_volume);
				float combined_volume = combined_AABB.get_volume();

				// Cost of creating a new parent for this node and the new leaf
				float cost = 2.0f * combined_volume;

				// Minimum cost of pushing the leaf further down the tree
				float inheritance_cost = 2.0f * (combined_volume - volume);

				// Cost of descending into child 0
				float cost_0;
				if (root->children[0]->is_leaf()) {
					AABB aabb(bounding_volume, root->children[0]->bounding_volume);
					cost_0 = aabb.get_volume() + inheritance_cost;
				} else {
					AABB aabb(bounding_volume, root->children[0]->bounding_volume);
					float old_volume = root->children[0]->bounding_volume.get_volume();
					float new_volume = aabb.get_volume();
					cost_0 = (new_volume - old_volume) + inheritance_cost;
				}

				// Cost of descending into child 1
				float cost_1;
				if (root->children[1]->is_leaf()) {
					AABB aabb(bounding_volume, root->children[1]->bounding_volume);
					cost_1 = aabb.get_volume() + inheritance_cost;
				} else {
					AABB aabb(bounding_volume, root->children[1]->bounding_volume);
					float old_volume = root->children[1]->bounding_volume.get_volume();
					float new_volume = aabb.get_volume();
					cost_1 = (new_volume - old_volume) + inheritance_cost;
				}

				// Descend according to the minimum cost.
				if (cost < cost_0 && cost < cost_1) {
					break;
				}

				// Descend
				if (cost_0 < cost_1) {
					root = root->children[0].get();
				} else {
					root = root->children[1].get();
				}
			}
			if (Node* parent = root->parent; !parent) {
				bvh_root = make_unique<Node>(BVH::Node::children_array_t{move(bvh_root), move(new_leaf_node)}, nullptr);
				bvh_root->children[0]->parent = bvh_root.get();
				bvh_root->children[1]->parent = bvh_root.get();
			} else {
				int root_idx = parent->get_index_of_child(root);
				unique_ptr<Node> new_intern_node = make_unique<Node>(BVH::Node::children_array_t{move(parent->children[root_idx]), move(new_leaf_node)}, root->parent);
				new_intern_node->children[0]->parent = new_intern_node.get();
				new_intern_node->children[1]->parent = new_intern_node.get();
				new_intern_node->parent->children[root_idx] = move(new_intern_node);
				root = root->parent;
				do {
					root->bounding_volume.enlarge_by(bounding_volume);
					root = root->parent;
				} while (root);
			}
		}
	}

	void BVH::remove(not_null<Collider_base*> collider) {
		remove_leaf(collider->transform.bvh_node);
		collider->transform.bvh_node = nullptr;
		erase_from_all_contacts(collider);
	}

	void BVH::remove_leaf(not_null<Node*> leaf) {
		if (!bvh_root) {
			return;
		}

		if (leaf == bvh_root.get()) {
			bvh_root.reset();
		} else {
			Node* grand_parent = leaf->parent->parent;
			int leaf_idx = leaf->get_index_in_parent();
			int bro_idx = 1 - leaf_idx;
			//int bro_idx = leaf->parent->get_index_of_brother(leaf);
			//int my_idx = 1 - bro_idx;
			auto unique_ptr_leaf = move(leaf->parent->children[leaf_idx]);// .reset(); // reset crapa. dc?
			auto brother = move(leaf->parent->children[bro_idx]);
			if (!grand_parent) {
				bvh_root = move(brother);
				bvh_root->parent = nullptr;
			} else {
				brother->parent = grand_parent;
				grand_parent->repalce_child(leaf->parent, move(brother));
				do {
					grand_parent->refit();
					grand_parent = grand_parent->parent;
				} while (grand_parent);
				/*
				* Maybe optimize upwards?
				*/
			}
		}
	}

	void BVH::generate_contacts(not_null<Collider_base*> collider, AABB bounding_volume) {
		if (bvh_root != nullptr) {
			stack<Node*, vector<Node*>> s;
			s.push(bvh_root.get());
			do {
				Node* node = s.top();
				s.pop();
				if (node->bounding_volume.does_intersect(bounding_volume)) {
					if (node->is_leaf()) {
						cache_contact(collider, node->bounded_object);
					} else {
						s.push(node->children[0].get());
						s.push(node->children[1].get());
					}
				}
			} while (!s.empty());
		}
	}

	void BVH::cache_contact(not_null<Collider_base*> collider0, not_null<Collider_base*> collider1) {
		if (collider0->get_inverse_mass() == 0 && collider1->get_inverse_mass() == 0) {
			return;
		}
		if (!less_for_unique_cache_entry(collider0, collider1)) {
			swap(collider0, collider1);
		}


		contacts_cache.insert(make_tuple(collider0, collider1));

		contacts_graph[collider0].insert(collider1);
		contacts_graph[collider1].insert(collider0);

		//assert(contacts_cache.size() == get_graph_size() / 2);
	}

	void BVH::remove_contact(not_null<Collider_base*> collider0, not_null<Collider_base*> collider1) {
		if (less_for_unique_cache_entry(collider0, collider1)) {
			contacts_cache.erase(make_tuple(collider0, collider1));
		} else {
			contacts_cache.erase(make_tuple(collider1, collider0));
		}
		contacts_graph[collider0].erase(collider1);
		contacts_graph[collider1].erase(collider0);
	}

	void BVH::print(std::ostream& out_stream) const {
		static int i = 1;
		out_stream << i++ << "\n";
		print_recursive(out_stream, bvh_root.get(), 0);
		out_stream << "\n";
	}

	void BVH::update(gsl::not_null<Collider_base*> collider) {
		remove(collider);
		insert(collider);
	}

	void BVH::cache_contacts(not_null<Collider_base*> collider) {
		if (bvh_root == nullptr) {
			return;
		}
		AABB aabb_for_game_obj(collider);
		queue<Node*> q;
		q.push(bvh_root.get());
		do {
			Node* n = q.front();
			q.pop();
			if (n->bounding_volume.does_intersect(aabb_for_game_obj)) {
				if (n->is_leaf()) {
					cache_contact(collider, n->bounded_object);
				} else {
					q.push(n->children[0].get());
					q.push(n->children[1].get());
				}
			}
		} while (!q.empty());
	}

	void BVH::cache_contacts_and_insert(not_null<Collider_base*> collider) {
		cache_contacts_and_insert_new(collider);
	}

	void BVH::cache_contacts_and_insert_old(not_null<Collider_base*> collider) {
		Node* best_insertion_place = bvh_root.get();
		AABB aabb_for_game_obj(collider);
		if (bvh_root != nullptr) {
			Node_greater greater(aabb_for_game_obj);
			queue<Node*> q;
			q.push(bvh_root.get());
			do {
				Node* node = q.front();
				q.pop();
				if (node->bounding_volume.does_intersect(aabb_for_game_obj)) {
					if (greater(node, best_insertion_place)) {
						best_insertion_place = node;
					}
					if (node->is_leaf()) {
						cache_contact(collider, node->bounded_object);
					} else {
						q.push(node->children[0].get());
						q.push(node->children[1].get());
					}
				}
			} while (!q.empty());
		}
		insert(best_insertion_place, collider, aabb_for_game_obj);
	}

	void BVH::cache_contacts_and_insert_new(gsl::not_null<Collider_base*> collider) {
		AABB aabb_for_game_obj(collider);
		if (bvh_root != nullptr) {
			stack<Node*, vector<Node*>> s;
			s.push(bvh_root.get());
			do {
				Node* node = s.top();
				s.pop();
				if (node->bounding_volume.does_intersect(aabb_for_game_obj)) {
					if (node->is_leaf()) {
						cache_contact(collider, node->bounded_object);
					} else {
						s.push(node->children[0].get());
						s.push(node->children[1].get());
					}
				}
			} while (!s.empty());
		}
		insert(bvh_root.get(), collider, aabb_for_game_obj);
	}

	void BVH::erase_from_all_contacts(not_null<Collider_base*> collider) {
		if (auto iterator_contacts = contacts_graph.find(collider); iterator_contacts != contacts_graph.end()) {
			for (not_null<Collider_base*> other : contacts_graph.at(collider)) {
				contacts_graph.at(other).erase(collider);
				if (contacts_graph.at(other).size() == 0) {
					contacts_graph.erase(other);
				}
				if (less_for_unique_cache_entry(collider, other)) {
					size_t removed = contacts_cache.erase(make_tuple(collider, other));
					assert(removed);
				} else {
					size_t removed = contacts_cache.erase(make_tuple(other, collider));
					assert(removed);
				}
			}
			contacts_graph.erase(collider);
		}
	}

	size_t BVH::get_contact_count() const {
		assert(contacts_cache.size() == get_graph_size()/2);
		return contacts_cache.size();
	}

	void BVH::clean_dirty_nodes() {
		clean_dirty_nodes_new();
	}

	void BVH::clean_dirty_nodes_old() {
		for (auto& node : dirty_nodes) {
			if (!node->bounding_volume.contains(node->bounded_object->provide_slim_aabb_parameters())) {
				update(node->bounded_object);
			}
		}
		dirty_nodes.clear();
	}

	void BVH::clean_dirty_nodes_new() {
		vector<Collider_base*> to_modify;
		to_modify.reserve(dirty_nodes.size());
		for (auto& node : dirty_nodes) {
			if (!node->bounding_volume.contains(node->bounded_object->provide_slim_aabb_parameters())) {
				to_modify.push_back(node->bounded_object);
				remove(node->bounded_object);
			}
		}
		dirty_nodes.clear();
		for (auto& obj : to_modify) {
			AABB aabb_for_node(obj);
			generate_contacts(obj, aabb_for_node);
			insert(bvh_root.get(), obj, aabb_for_node);
		}
	}

	const unordered_set<not_null<Collider_base*>> BVH::get_objects_in_contact_with(not_null<Collider_base*> collider) const {
		auto it = contacts_graph.find(collider);
		if (it != contacts_graph.end()) {
			return contacts_graph.at(collider);
		} else {
			return unordered_set<not_null<Collider_base*>>();
		}
	}

	void BVH::print_recursive(std::ostream& out_stream, Node* root, int level) const {
		for (int i = 0; i < level; i++) {
			out_stream << "|\t";
		}
		vec3 pos = root->bounding_volume.get_center();
		//outStream << meshIdToIdNames[root->boundedObject->mesh->GetId()] << " " << pos;
		out_stream << pos << " ";
		if (!root->is_leaf()) {
			out_stream << "{\n";
			level++;
			print_recursive(out_stream, root->children[0].get(), level);
			print_recursive(out_stream, root->children[1].get(), level);
			for (int i = 0; i < level - 1; i++) {
				out_stream << "|\t";
			}
			out_stream << "}\n";
		} else {
#ifdef DEBUGGING
			out_stream << root->bounded_object->name << "\n";
#endif // DEBUGGING
		}
	}

	void BVH::mark_dirty_node(gsl::not_null<Node*> node) {
		dirty_nodes.insert(node);
	}

	BVH::Manhattan_distance_greater::Manhattan_distance_greater(AABB added_volume) :
		added_volume(added_volume) {
	}

	float migine::BVH::Manhattan_distance_greater::get_distance(not_null<const Node*> to) {
		auto it = cache.find(to);
		if (it != cache.end()) {
			return it->second;
		}
		vec3 added_volume_center = added_volume.get_center();
		vec3 other_center = to->bounding_volume.get_center();
		float distance = abs(added_volume_center.x - other_center.x) +
			abs(added_volume_center.y - other_center.y) +
			abs(added_volume_center.z - other_center.z);
		cache[to] = distance;
		return distance;
	}

	bool BVH::Manhattan_distance_greater::operator()(const Node* lhs, const Node* rhs) {
		return get_distance(lhs) > get_distance(rhs);
	}


	int BVH::get_graph_size() const {
		int sum = 0;
		for (auto& [_, line] : contacts_graph) {
			sum += line.size();
		}
		return sum;
	}

	const BVH::contacts_cache_t& BVH::get_contacts() const {
		return contacts_cache;
	}

#ifdef DEBUGGING
	void BVH::render_all(const Camera& camera) const {
		render_all_recursive(camera, bvh_root.get());
	}

	bool BVH::less_for_unique_cache_entry(not_null<const Collider_base*> lhs, not_null<const Collider_base*> rhs) const {
		return lhs < rhs;
	}

	void BVH::render_all_recursive(const Camera& camera, Node* root) const {
		if (root == nullptr) {
			return;
		}
		root->bounding_volume.render(camera);
		if (!root->is_leaf()) {
			render_all_recursive(camera, root->children[0].get());
			render_all_recursive(camera, root->children[1].get());
		}
	}
#endif
}