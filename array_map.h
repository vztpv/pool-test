#ifndef ARRAY_MAP_H
#define ARRAY_MAP_H

#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <utility>
#include <execution>

// like set, map,   instead of map!
namespace wiz {

	template <class T>
	class ASC {
	public:
		bool operator() (const T& t1, const T& t2) const { return t1 < t2; }
	};

	template <class T>
	class EQ { // EE -> EQ!
	public:
		bool operator() (const T& t1, const T& t2) const { return t1 == t2; }
	};

	template <class T>
	inline int test(const T& t1, const T& t2) noexcept {
		if (t1 < t2) {
			return -1;
		}
		else if (t2 < t1) {
			return 1;
		}
		else {
			return 0;
		}
	}

	template <>
	inline int test(const std::string& str1, const std::string& str2) noexcept
	{
		return str1.compare(str2); 
	}

	template <class Key, class Data>
	class array_map {
		typedef enum _Color : uint8_t { BLACK = 0, RED = 1 } Color;

		using array_map_int = uint64_t;

		template < class Key, class Data >
		class RB_Node // no REMOVE method!
		{
		public:
			Key first;
			Data second;
			array_map_int id = 0; // NULL value? id == -1 ?
			array_map_int left = 0;
			array_map_int right = 0;
			array_map_int p = 0; // parent
			Color color = BLACK;
		public:
			void Clear()
			{
				id = 0;
				first = Key();
				second = Data();
				left = 0;
				right = 0;
				p = 0;
				color = RED;
			}
		public:
			explicit RB_Node(const Key& key = Key(), const Data& data = Data()) : first(key), second(data), color(BLACK) {}

		public:
			bool operator==(const RB_Node<Key, Data>& other)const {
				return this->id == other.id && other.id != -1;
			}
			bool operator<(const RB_Node<Key, Data>& other) const {
				return this->first < other.first;
			}
		};


		template <class T>
		class TEST // need to rename!
		{
		public:
			inline int operator()(const T& t1, const T& t2) const noexcept {
				return test(t1, t2);
			}
		};


		template < class Key, class Data, class COMP = ASC<Key>, class COMP2 = EQ<Key>, class COMP3 = TEST<Key>>
		class RB_Tree
		{
		private:
			inline  bool IsNULL(const RB_Node<Key, Data>& x)const noexcept
			{
				return x.id <= 0 || x.id > arr.size();
			}
			inline  bool IsNULL(const array_map_int id) const noexcept
			{
				return id <= 0 || id > arr.size();
			}

			void Clear()
			{
				arr.clear();
				arr.push_back(RB_Node<Key, Data>());
				root = 0;
				count = 0;

				remain_list.clear();
			}

		public:
			std::vector<RB_Node<Key, Data>> arr = { RB_Node<Key, Data>() };
		private:
			std::vector<std::pair<Key, Data>> remain_list;
			std::vector<uint8_t> dead;
			std::vector<array_map_int> next;
			array_map_int root = 0;
			array_map_int count = 0;
			array_map_int dead_list = 0;
			array_map_int max_idx = 0;
			array_map_int min_idx = 0;
			bool first_time = true;
		public:
			explicit RB_Tree() {}
			~RB_Tree() {
				//
			}
			void reserve(size_t num) {
				arr.reserve(num + 1);
				dead.reserve(num + 1);
				next.reserve(num + 1);
				//remain_list.reserve(num);
			}
			void reserve_remain(size_t num) {
				remain_list.reserve(num);
			}
			using iterator = typename std::vector<RB_Node<Key, Data>>::iterator;
			using const_iterator = typename std::vector<RB_Node<Key, Data>>::const_iterator;

			iterator begin() {
				return arr.begin() + 1;
			}
			const_iterator begin() const {
				return arr.cbegin() + 1;
			}
			iterator end() {
				return arr.end();
			}
			const_iterator end() const {
				return arr.cend();
			}
		private:// LEFT ROTATE Tree x
			void LEFT_ROTATE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x) {
				//if( x !=NIL<T>::Get() || tree.root != NIL<T>::Get() ) { return; }
				RB_Node<Key, Data>* y;
				// y = right[x]
				y = &(tree->arr[x->right]);
				// right[x] = left[y]
				x->right = y->left;
				// if(left[y] != nil[T]
				if (!IsNULL(tree->arr[y->left])) {
					//    then p[left[y]] = x
					tree->arr[y->left].p = x->id;
				}
				// p[y] = p[x]
				y->p = x->p;

				// if( p[x] = nil[T]
				if (IsNULL(tree->arr[x->p])) {
					//  then root[T] = y
					tree->root = y->id;
				}
				//  else if x = left[p[x]]
				else if (x == &(tree->arr[tree->arr[x->p].left])) {
					//      then left[p[x]] = y
					tree->arr[x->p].left = y->id;
				}
				//  else right[p[x]] = y
				else { tree->arr[x->p].right = y->id; }
				// left[y] = x
				y->left = x->id;
				// p[x] = y
				x->p = y->id;
			}
			// Right ROTATE Tree x // left <> right
			void RIGHT_ROTATE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x) {
				//if( x !=NIL<T>::Get() || tree.root != NIL<T>::Get() ) { return; }

				RB_Node<Key, Data>* y;
				// y = right[x]
				y = &(tree->arr[x->left]);
				// right[x] = left[y]
				x->left = y->right;
				// if(left[y] != nil[T]
				if (!IsNULL(tree->arr[y->right])) {
					//    then p[left[y]] = x
					tree->arr[y->right].p = x->id;
				}
				// p[y] = p[x]
				y->p = x->p;

				// if( p[x] = nil[T]
				if (IsNULL(tree->arr[x->p])) {
					//  then root[T] = y
					tree->root = y->id;
				}
				//  else if x = left[p[x]]
				else if (x == &(tree->arr[tree->arr[x->p].right])) {
					//      then left[p[x]] = y
					tree->arr[x->p].right = y->id;
				}
				//  else right[p[x]] = y
				else { tree->arr[x->p].left = y->id; }
				// left[y] = x
				y->right = x->id;
				// p[x] = y
				x->p = y->id;
			}
			const RB_Node<Key, Data>* SEARCH(const RB_Node<Key, Data>* x, const Key& k) const
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &arr[x->left];
					}
					else if (temp > 0) {
						x = &arr[x->right];
					}
					else {
						break;
					}
				}

				return x;
			}



			RB_Node<Key, Data>* SEARCH(RB_Node<Key, Data>* x, const Key& k)
			{
				COMP3 comp3;
				//array_map_int count = 0;
				while (x->id) { // != nil
					//count++;
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &arr[x->left];
					}
					else if (temp > 0) {
						x = &arr[x->right];
					}
					else {
						break;
					}
				}
				//std::cout << count << "\n";
				return x;
			}



			const RB_Node<Key, Data>* SEARCH(const RB_Node<Key, Data>* x, Key&& k, Key* temp) const
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &arr[x->left];
					}
					else if (temp > 0) {
						x = &arr[x->right];
					}
					else {
						break;
					}
				}
				if (temp) {
					*temp = std::move(k);
				}
				return x;
			}
			RB_Node<Key, Data>* SEARCH(RB_Node<Key, Data>* x, Key&& k, Key* temp)
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &arr[x->left];
					}
					else if (temp > 0) {
						x = &arr[x->right];
					}
					else {
						break;
					}
				}
				if (temp) {
					*temp = std::move(k);
				}
				return x;
			}
			void INSERT_FIXUP(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* z) /// google ..
			{
				RB_Node<Key, Data>* y;
				while (tree->arr[z->p].color == RED) {
					if (z->p == tree->arr[tree->arr[z->p].p].left) {
						y = &(tree->arr[tree->arr[tree->arr[z->p].p].right]);
						if (y->color == RED) {
							tree->arr[z->p].color = BLACK;
							y->color = BLACK;
							tree->arr[tree->arr[z->p].p].color = RED;
							z = &(tree->arr[tree->arr[z->p].p]);
						}
						else
						{
							if (z == &(tree->arr[tree->arr[z->p].right])) {
								z = &(tree->arr[z->p]);
								LEFT_ROTATE(tree, z);
							}
							tree->arr[z->p].color = BLACK;
							tree->arr[tree->arr[z->p].p].color = RED;
							RIGHT_ROTATE(tree, &(tree->arr[tree->arr[z->p].p]));
						}
					}
					else {
						y = &(tree->arr[arr[tree->arr[z->p].p].left]);
						if (y->color == RED) {
							tree->arr[z->p].color = BLACK;
							y->color = BLACK;
							tree->arr[tree->arr[z->p].p].color = RED;
							z = &(tree->arr[tree->arr[z->p].p]);
						}
						else {
							if (z == &(tree->arr[tree->arr[z->p].left])) {
								z = &(tree->arr[z->p]);
								RIGHT_ROTATE(tree, z);
							}
							tree->arr[z->p].color = BLACK;
							tree->arr[tree->arr[z->p].p].color = RED;
							LEFT_ROTATE(tree, &(tree->arr[tree->arr[z->p].p]));
						}
					}
				}
				tree->arr[tree->root].color = BLACK;
			}
			array_map_int INSERT(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key, array_map_int hint)
			{
				COMP comp;
				COMP2 eq;
				COMP3 comp3;

				array_map_int y_idx = 0;
				array_map_int x_idx = tree->root;
				auto& chk = tree->arr;

				size_t now = tree->arr.size(); //

				int select = 0; // 0 : nothing, 1: min, 2: max

				if (!IsNULL(tree->root)) {
					if (hint) {
						x_idx = hint;
					}
					else {
						if (tree->arr[min_idx].first > key.first) {
							select = 1;
							hint = min_idx;
						}
						else if (tree->arr[max_idx].first < key.first) {
							select = 2;
							hint = max_idx;
						}
					}

					if (hint) {
						x_idx = hint;
					}

					while (//!IsNULL(tree->arr[x_idx]) &&
						!IsNULL(x_idx) //&& !hint
						)
					{
						y_idx = x_idx;
						int test = comp3(key.first, tree->arr[x_idx].first);

						if (test < 0)
						{
							x_idx = tree->arr[x_idx].left;
						}
						else if (test > 0) {
							x_idx = tree->arr[x_idx].right;
						}
						else {
							break;
						}
					}

					if (!IsNULL(x_idx) && eq(key.first, tree->arr[x_idx].first)) {
						tree->arr[x_idx].first = (key.first);
						tree->arr[x_idx].second = (key.second);
						return x_idx;
					}
				}
				else {
					min_idx = now;
					max_idx = now;
				}

				RB_Node<Key, Data>* z = nullptr;
				if (0 == tree->dead_list) {
					tree->arr.push_back(RB_Node<Key, Data>());
					tree->dead.push_back(0);
					tree->next.push_back(0);
					tree->arr.back().id = now;
					tree->arr.back().first = (key.first);
					tree->arr.back().second = (key.second);

					z = &(tree->arr.back());
				}
				else {
					now = tree->dead_list;
					tree->dead_list = tree->next[now];
					array_map_int id = tree->arr[now].id;
					array_map_int next = tree->next[now];

					tree->arr[now].Clear();
					tree->arr[now].id = id;
					tree->arr[now].first = (key.first);
					tree->arr[now].second = (key.second);
					tree->next[now] = next;
					tree->dead[now] = false;

					z = &(tree->arr[now]);
				}

				z->p = tree->arr[y_idx].id;

				if (IsNULL(tree->arr[y_idx])) {
					tree->root = z->id;
				}
				else if (comp(z->first, tree->arr[y_idx].first)) { // comp3? 
					tree->arr[y_idx].left = z->id;//
				}
				else {
					tree->arr[y_idx].right = z->id;//
				}

				z->left = 0; // = nil
				z->right = 0;
				z->color = RED; // = RED

				// insert fixup
				INSERT_FIXUP(tree, z);

				count++;

				if (select == 1) {
					min_idx = z->id;
				}
				else if (select == 2) {
					max_idx = z->id;
				}
				return z->id;
			}
			array_map_int INSERT(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key, array_map_int hint)
			{

				COMP comp;
				COMP2 eq;
				COMP3 comp3;

				array_map_int y_idx = 0;
				array_map_int x_idx = tree->root;
				auto& chk = tree->arr;

				size_t now = tree->arr.size(); //

				int select = 0; // 0 : nothing, 1: min, 2: max

				if (!IsNULL(tree->root)) {
					if (hint) {
						x_idx = hint;
					}
					else {
						if (tree->arr[min_idx].first > key.first) {
							select = 1;
							hint = min_idx;
						}
						else if (tree->arr[max_idx].first < key.first) {
							select = 2;
							hint = max_idx;
						}
					}

					if (hint) {
						x_idx = hint;
					}

					while (//!IsNULL(tree->arr[x_idx]) &&
						!IsNULL(x_idx) //&& !hint
						)
					{
						y_idx = x_idx;
						int test = comp3(key.first, tree->arr[x_idx].first);

						if (test < 0)
						{
							x_idx = tree->arr[x_idx].left;
						}
						else if (test > 0) {
							x_idx = tree->arr[x_idx].right;
						}
						else {
							break;
						}
					}

					if (!IsNULL(x_idx) && eq(key.first, tree->arr[x_idx].first)) {
						tree->arr[x_idx].first = std::move(key.first);
						tree->arr[x_idx].second = std::move(key.second);
						return x_idx;
					}
				}
				else {
					min_idx = now;
					max_idx = now;
				}

				RB_Node<Key, Data>* z = nullptr;
				if (0 == tree->dead_list) {
					tree->arr.push_back(RB_Node<Key, Data>());
					tree->dead.push_back(0);
					tree->next.push_back(0);
					tree->arr.back().id = now;
					tree->arr.back().first = std::move(key.first);
					tree->arr.back().second = std::move(key.second);

					z = &(tree->arr.back());
				}
				else {
					now = tree->dead_list;
					tree->dead_list = tree->next[now];
					array_map_int id = tree->arr[now].id;
					array_map_int next = tree->next[now];

					tree->arr[now].Clear();
					tree->arr[now].id = id;
					tree->arr[now].first = std::move(key.first);
					tree->arr[now].second = std::move(key.second);
					tree->next[now] = next;
					tree->dead[now] = false;

					z = &(tree->arr[now]);
				}

				z->p = tree->arr[y_idx].id;

				if (IsNULL(tree->arr[y_idx])) {
					tree->root = z->id;
				}
				else if (comp(z->first, tree->arr[y_idx].first)) { // comp3? 
					tree->arr[y_idx].left = z->id;//
				}
				else {
					tree->arr[y_idx].right = z->id;//
				}

				z->left = 0; // = nil
				z->right = 0;
				z->color = RED; // = RED

				// insert fixup
				INSERT_FIXUP(tree, z);

				count++;

				if (select == 1) {
					min_idx = z->id;
				}
				else if (select == 2) {
					max_idx = z->id;
				}
				return z->id;
			}
			void LAZYINSERT(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key) {
				tree->remain_list.push_back(key);
			}
			void LAZYINSERT(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key) {
				tree->remain_list.push_back(std::move(key));
			}
			void PrintTree(RB_Tree<Key, Data, COMP>* tree) {
				if (tree->empty()) { return; }
				std::queue<int> que;

				que.push(tree->root);
				std::cout << "chk " << "\n";

				while (!que.empty()) {
					int id = que.front(); que.pop();

					std::cout << tree->arr[id].p << " " << id << "\n";

					if (!IsNULL(tree->arr[id].left)) {
						que.push(tree->arr[id].left);
					}
					else {
						std::cout << " X ";
					}
					if (!IsNULL(tree->arr[id].right)) {
						que.push(tree->arr[id].right);
					}
					else {
						std::cout << "X ";
					}
				}
			}
		private:

			void _test(int64_t left, int64_t right, int64_t hint, RB_Tree<Key, Data, COMP>* tree, std::vector<std::pair<Key, Data>>* vec) {
				if (left > right) {
					return;
				}

				size_t middle = (left + right) / 2;

				hint = INSERT(tree, std::move((*vec)[middle]), hint);

				_test(left, middle - 1, hint, tree, vec);
				_test(middle + 1, right, hint, tree, vec);
			}
			// [left, right) ?
			void _test2(int64_t left, int64_t right, int64_t hint, RB_Tree<Key, Data, COMP>* tree, std::vector<std::pair<Key, Data>>* vec) {
				for (int64_t i = left; i < right; ++i) {
					INSERT(tree, std::move((*vec)[i]), 0);
				}
			}

			void REALINSERT(RB_Tree<Key, Data, COMP>* tree) {

				if (tree->remain_list.empty()) {
					return;
				}

				std::stable_sort(std::execution::par, tree->remain_list.begin(), tree->remain_list.end());

				// remove dupplication? but no remove last dup?

				std::vector<std::pair<Key, Data>> vec;
				vec.reserve(tree->remain_list.size());

				bool first = true;
				std::pair<Key, Data> before;
				for (auto&& x : tree->remain_list) {
					if (!first && before.first == x.first) {
						continue;
					}
					before = x;
					first = false;
					vec.push_back(std::move(x));

					//INSERT(tree, std::move(x));
				}
				tree->remain_list.clear();

				// 
				if (first_time) {
					_test(0, vec.size() - 1, 0, tree, &vec);
				}
				else {
					_test2(0, vec.size(), 0, tree, &vec);
				}
				first_time = false;

				return;
			}

			RB_Node<Key, Data>* MAXIMUM(RB_Node<Key, Data>* x) {
				while (!IsNULL(x->right)) {
					x = &arr[x->right];
				}
				return x;
			}
			RB_Node<Key, Data>* MINIMUM(RB_Node<Key, Data>* x)
			{
				while (!IsNULL(x->left)) { // != nil
					x = &arr[x->left];
				}
				return x;
			}
			RB_Node<Key, Data>* SUCCESSOR(RB_Node<Key, Data>* x)
			{
				if (!IsNULL(x->right)) { // nil
					return MINIMUM(&arr[x->right]);
				}

				RB_Node<Key, Data>* y = &arr[x->p];
				while (!IsNULL(y->id) && x == &arr[y->right])
				{
					x = y; y = &arr[y->p];
				}
				return y;
			}

			void REMOVE_FIXUP(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x)
			{
				RB_Node<Key, Data>* w;

				while (x->id != root && x->color == BLACK)
				{
					if (x == &(arr[arr[x->p].left])) {
						w = &(arr[arr[x->p].right]);
						if (w->color == RED) {
							w->color = BLACK;
							arr[x->p].color = RED;
							LEFT_ROTATE(tree, &arr[x->p]);
							w = &arr[arr[x->p].right];
						}
						if (arr[w->left].color == BLACK && arr[w->right].color == BLACK) {
							w->color = RED;
							x = &arr[x->p];
						}
						else {
							if (arr[w->right].color == BLACK) {
								arr[w->left].color = BLACK;
								w->color = RED;
								RIGHT_ROTATE(tree, w);
								w = &arr[arr[x->p].right];
							}
							w->color = arr[x->p].color;
							arr[x->p].color = BLACK;
							arr[w->right].color = BLACK;
							LEFT_ROTATE(tree, &arr[x->p]);
							x = &arr[root];
						}
					}
					else {
						w = &arr[arr[x->p].left];
						if (w->color == RED) {
							w->color = BLACK;
							arr[x->p].color = RED;
							RIGHT_ROTATE(tree, &arr[x->p]);
							w = &(arr[arr[x->p].left]);
						}
						if (arr[w->left].color == BLACK && arr[w->right].color == BLACK) {
							w->color = RED;
							x = &arr[x->p];
						}
						else {
							if (arr[w->left].color == BLACK) {
								arr[w->right].color = BLACK;
								w->color = RED;
								LEFT_ROTATE(tree, w);
								w = &(arr[arr[x->p].left]);
							}
							w->color = arr[x->p].color;
							arr[x->p].color = BLACK;
							arr[w->left].color = BLACK;
							RIGHT_ROTATE(tree, &arr[x->p]);
							x = &arr[root];
						}
					}
				}
				x->color = BLACK;
			}


			RB_Node<Key, Data>* REMOVE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* z)
			{
				RB_Node<Key, Data>* x;
				RB_Node<Key, Data>* y;

				if (IsNULL(z->left)
					|| IsNULL(z->right)) {
					y = z;
				}
				else { y = SUCCESSOR(z); }

				if (!IsNULL(y->left)) {
					x = &(tree->arr[y->left]);
				}
				else { x = &(tree->arr[y->right]); }

				x->p = y->p;

				if (IsNULL(y->p)) {
					tree->root = x->id;
				}
				else if (y == &(arr[arr[y->p].left])) {
					arr[y->p].left = x->id;
				}
				else { arr[y->p].right = x->id; }

				if (y != z) { //important part!
					z->first = std::move(y->first); // chk??
					z->second = std::move(y->second);
					std::swap(z->dead, y->dead);
					std::swap(z->next, y->next);
				}
				if (y->color == BLACK) {
					REMOVE_FIXUP(tree, x);
				}
				return y;
			}
		public:
			void PrintTree() {
				PrintTree(this);
			}
			void LazyInsert(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key) {
				LAZYINSERT(this, key);
			}
			void LazyInsert(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key) {
				LAZYINSERT(this, std::move(key));
			}
			// insert, search, remove.
			array_map_int Insert(const std::pair<Key, Data>& key)
			{
				return INSERT(this, key, 0);
			}
			array_map_int Insert(std::pair<Key, Data>&& key)
			{
				return INSERT(this, key, 0);
			}

			bool IsExist(const Key& key) const
			{
				// NULL != ~
				return !IsNULL(*SEARCH(&arr[root], key));
			}
			bool IsExist(Key&& key) const
			{
				// NULL != ~
				return !IsNULL(*SEARCH(&arr[root], key));
			}


			RB_Node<Key, Data>* Search(const Key& key, array_map_int* id = nullptr) {
				RB_Node<Key, Data>* x = &arr[root];


				x = SEARCH(x, key);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			const RB_Node<Key, Data>* Search(const Key& key, array_map_int* id = nullptr) const {
				const RB_Node<Key, Data>* x = &arr[root];

				x = SEARCH(x, key);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			RB_Node<Key, Data>* Search(Key&& key, array_map_int* id = nullptr, Key* temp = nullptr) {
				RB_Node<Key, Data>* x = SEARCH(&arr[root], std::move(key), temp);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			const RB_Node<Key, Data>* Search(Key&& key, array_map_int* id = nullptr, Key* temp = nullptr) const {
				RB_Node<Key, Data>* x = &arr[root];


				x = SEARCH(temp, key);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			void RealInsert()
			{
				REALINSERT(this);
			}

			void Remove(const Key& key)
			{
				RB_Node<Key, Data>* node = SEARCH(&arr[root], key);

				if (!IsNULL(*node))
				{

					RB_Node<Key, Data>* temp = REMOVE(this, node);

					temp->dead = true;
					temp->next = this->dead_list;
					this->dead_list = temp->id;
					count--;
				}
			}

			void Remove(Key&& key)
			{
				RB_Node<Key, Data>* node = SEARCH(&arr[root], std::move(key));

				if (!IsNULL(*node))
				{
					RB_Node<Key, Data>* temp = REMOVE(this, node);

					temp->dead = true;
					temp->next = this->dead_list;
					this->dead_list = temp->id;
					count--;
				}
			}

			Data& DataVal(array_map_int idx) {
				return arr[idx].second;
			}
			const Data& DataVal(array_map_int idx) const {
				return arr[idx].second;
			}

			bool IsEmpty() const
			{
				return 0 == count;
			}
			bool empty() const { return IsEmpty(); }
			array_map_int GetCount() const
			{
				return count;
			}
			array_map_int size() const { return count; }
			void clear() {
				Clear();
			}
		};

	public:
		using iterator = typename std::vector<RB_Node<Key, Data>>::iterator;
		using const_iterator = typename std::vector<RB_Node<Key, Data>>::const_iterator;
	private:
		mutable RB_Tree<Key, Data> arr;
	public:
		explicit array_map(size_t reserve_num = 0) {
			if (reserve_num > 0) {
				arr.reserve(reserve_num);
			}
		}

	public:
		void PrintTree() {
			arr.PrintTree();
		}
		bool empty() const {
			arr.RealInsert();

			return arr.empty();
		}
		auto size() const {
			arr.RealInsert();

			return arr.size();
		}
		void clear() {
			arr.clear();
		}
		void reserve(array_map_int x) {
			arr.reserve(x);
		}
		void reserve2(array_map_int x) {
			arr.reserve_remain(x);
		}
	public:
		iterator begin() {
			arr.RealInsert();

			return arr.begin();
		}
		iterator end() {
			arr.RealInsert();

			return arr.end();
		}
		const_iterator begin() const {
			arr.RealInsert();

			return arr.begin();
		}
		const_iterator end() const {
			arr.RealInsert();

			return arr.end();
		}
		iterator find(const Key& key) {
			arr.RealInsert();

			array_map_int id;
			const RB_Node<Key, Data>* x = arr.Search(key, &id);
			if (0 == x->id) {
				return arr.end();
			}

			auto result = begin() + (x->id - 1);

			return result;
		}
		const_iterator find(const Key& key) const {
			arr.RealInsert();

			array_map_int id;
			RB_Node<Key, Data>* x = arr.Search(key, &id);
			if (0 == x->id) {
				return arr.end();
			}
			return arr.begin() + (x->id - 1);
		}
		iterator find(Key&& key) {
			arr.RealInsert();

			array_map_int id;
			RB_Node<Key, Data>* x = arr.Search(std::move(key), &id);
			if (0 == x->id) {
				return arr.end();
			}
			return begin() + (x->id - 1);
		}
		const_iterator find(Key&& key) const {
			arr.RealInsert();

			array_map_int id;
			RB_Node<Key, Data>* x = arr.Search(std::move(key), &id);
			if (0 == x->id) {
				return arr.end();
			}
			return arr.begin() + (x->id - 1);
		}

	public:
		// different poarray_map_int compared by std::map?
		void insert(const std::pair<Key, Data>& value) {
			lazy_insert(value);

			//	arr.RealInsert();

			//	arr.Insert(wiz::Pair<Key, Data>(value.first, value.second));
		}
		void insert(std::pair<Key, Data>&& value) {
			lazy_insert(value);

			//arr.RealInsert();

		//	arr.Insert(wiz::Pair<Key, Data>(value.first, value.second));
		}
		void update()
		{
			arr.RealInsert();
		}
		void lazy_insert(const std::pair<Key, Data>& value) {
			arr.LazyInsert(&arr, value);
		}
		void lazy_insert(std::pair<Key, Data>&& value) {
			arr.LazyInsert(&arr, std::move(value));
		}

		void remove(const std::pair<Key, Data>& value)
		{
			arr.RealInsert();

			arr.Remove(value.first);
		}
		void remove(std::pair<Key, Data>&& value)
		{
			arr.RealInsert();

			arr.Remove(std::move(value.first));
		}
		Data& at(const Key& key) {
			return (*this)[key];
		}
		const Data& at(const Key& key) const {
			return find(key)->second;
		}
		Data& at(Key&& key) {
			return (*this)[std::move(key)];
		}
		const Data& at(Key&& key) const {
			return find(std::move(key))->second;
		}

		Data& operator[](const Key& key) {
			arr.RealInsert();

			RB_Node<Key, Data>* idx = (arr.Search(key));

			if (0 == idx->id) {
				array_map_int _idx = arr.Insert(std::pair<Key, Data>(key, Data())); //// return positon? - to do
				return arr.DataVal(_idx);
			}
			else {
				return idx->second;
			}
		}

		const Data& operator[](const Key& key) const {
			arr.RealInsert();

			const RB_Node<Key, Data>* idx = arr.Search(key);
			if (0 == idx->id) {
				//array_map_int _idx = arr.Insert(wiz::Pair<Key, Data>(key, Data())); //// return positon? - to do
				//return arr.DataVal(_idx);
				throw "do not exist in array_map";
			}
			else {
				return idx->second;
			}
		}

		Data& operator[](Key&& key) {
			arr.RealInsert();
			Key temp;

			RB_Node<Key, Data>* idx = arr.Search(std::move(key), nullptr, &temp);
			if (0 == idx->id) {
				array_map_int _idx = arr.Insert(std::pair<Key, Data>(std::move(temp), Data())); //// return positon? - to do
				return arr.DataVal(_idx);
			}
			else {
				return idx->second;
			}
		}

		const Data& operator[](Key&& key) const {
			arr.RealInsert();

			const RB_Node<Key, Data>* idx = arr.Search(std::move(key));
			if (0 == idx->id) {
				//array_map_int _idx = arr.Insert(wiz::Pair<Key, Data>(std::move(key), Data())); //// return positon? - to do
				//return arr.Idx(_idx).second;
				throw "do not exist in array_map";
			}
			else {
				return idx->second;
			}
		}

	};
}
namespace clau {

	template <class T>
	class ASC {
	public:
		bool operator() (const T& t1, const T& t2) const { return t1 < t2; }
	};

	template <class T>
	class EQ { // EE -> EQ!
	public:
		bool operator() (const T& t1, const T& t2) const { return t1 == t2; }
	};

	template <class T>
	inline int test(const T& t1, const T& t2) noexcept {
		if (t1 < t2) {
			return -1;
		}
		else if (t2 < t1) {
			return 1;
		}
		else {
			return 0;
		}
	}

	template <>
	inline int test(const std::string& str1, const std::string& str2) noexcept
	{
		return str1.compare(str2);
	}

	template <class Key, class Data, class Arena, template<class> class Allocator>
	class array_map {
		typedef enum _Color : uint8_t { BLACK = 0, RED = 1 } Color;

		using array_map_int = uint64_t;

		template < class Key, class Data >
		class RB_Node // no REMOVE method!
		{
		public:
			Key first;
			Data second;
			array_map_int id = 0; // NULL value? id == -1 ?
			array_map_int left = 0;
			array_map_int right = 0;
			array_map_int p = 0; // parent
			Color color = BLACK;
		public:
			void Clear()
			{
				id = 0;
				first = Key();
				second = Data();
				left = 0;
				right = 0;
				p = 0;
				color = RED;
			}
		public:
			explicit RB_Node(const Key& key = Key(), const Data& data = Data()) : first(key), second(data), color(BLACK) {}

		public:
			bool operator==(const RB_Node<Key, Data>& other)const {
				return this->id == other.id && other.id != -1;
			}
			bool operator<(const RB_Node<Key, Data>& other) const {
				return this->first < other.first;
			}
		};


		template <class T>
		class TEST // need to rename!
		{
		public:
			TEST() {
				//
			}
			inline int operator()(const T& t1, const T& t2) const noexcept {
				return test(t1, t2);
			}
		};


		template <class Key, class Data, class COMP = ASC<Key>, class COMP2 = EQ<Key>, class COMP3 = TEST<Key>>
		class RB_Tree
		{
		private:
			inline  bool IsNULL(const RB_Node<Key, Data>& x)const noexcept
			{
				return x.id <= 0 || x.id > _arr.size();
			}
			inline  bool IsNULL(const array_map_int id) const noexcept
			{
				return id <= 0 || id > _arr.size();
			}

			void Clear()
			{
				_arr.clear();
				_arr.push_back(RB_Node<Key, Data>());
				root = 0;
				count = 0;

				remain_list.clear();
			}

		public:
			std::vector<RB_Node<Key, Data>, Allocator<RB_Node<Key, Data>>> _arr;
		private:
			std::vector<std::pair<Key, Data>> remain_list;
			std::vector<uint8_t> dead;
			std::vector<array_map_int> next;
			array_map_int root = 0;
			array_map_int count = 0;
			array_map_int dead_list = 0;
			array_map_int max_idx = 0;
			array_map_int min_idx = 0;
			bool first_time = true;
		public:
			RB_Tree() {
				//
			}
			explicit RB_Tree(Allocator<RB_Node<Key, Data>>& alloc) : _arr(alloc) {  
				_arr.push_back(RB_Node<Key, Data>());
			}
			~RB_Tree() {
				//
			}
			void reserve(size_t num) {
				_arr.reserve(num + 1);
				dead.reserve(num + 1);
				next.reserve(num + 1);
				//remain_list.reserve(num);
			}
			void reserve_remain(size_t num) {
				remain_list.reserve(num);
			}
			using iterator = typename std::vector<RB_Node<Key, Data>>::iterator;
			using const_iterator = typename std::vector<RB_Node<Key, Data>>::const_iterator;

			iterator begin() {
				return _arr.begin() + 1;
			}
			const_iterator begin() const {
				return _arr.cbegin() + 1;
			}
			iterator end() {
				return _arr.end();
			}
			const_iterator end() const {
				return _arr.cend();
			}
		private:// LEFT ROTATE Tree x
			void LEFT_ROTATE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x) {
				//if( x !=NIL<T>::Get() || tree.root != NIL<T>::Get() ) { return; }
				RB_Node<Key, Data>* y;
				// y = right[x]
				y = &(tree->_arr[x->right]);
				// right[x] = left[y]
				x->right = y->left;
				// if(left[y] != nil[T]
				if (!IsNULL(tree->_arr[y->left])) {
					//    then p[left[y]] = x
					tree->_arr[y->left].p = x->id;
				}
				// p[y] = p[x]
				y->p = x->p;

				// if( p[x] = nil[T]
				if (IsNULL(tree->_arr[x->p])) {
					//  then root[T] = y
					tree->root = y->id;
				}
				//  else if x = left[p[x]]
				else if (x == &(tree->_arr[tree->_arr[x->p].left])) {
					//      then left[p[x]] = y
					tree->_arr[x->p].left = y->id;
				}
				//  else right[p[x]] = y
				else { tree->_arr[x->p].right = y->id; }
				// left[y] = x
				y->left = x->id;
				// p[x] = y
				x->p = y->id;
			}
			// Right ROTATE Tree x // left <> right
			void RIGHT_ROTATE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x) {
				//if( x !=NIL<T>::Get() || tree.root != NIL<T>::Get() ) { return; }

				RB_Node<Key, Data>* y;
				// y = right[x]
				y = &(tree->_arr[x->left]);
				// right[x] = left[y]
				x->left = y->right;
				// if(left[y] != nil[T]
				if (!IsNULL(tree->_arr[y->right])) {
					//    then p[left[y]] = x
					tree->_arr[y->right].p = x->id;
				}
				// p[y] = p[x]
				y->p = x->p;

				// if( p[x] = nil[T]
				if (IsNULL(tree->_arr[x->p])) {
					//  then root[T] = y
					tree->root = y->id;
				}
				//  else if x = left[p[x]]
				else if (x == &(tree->_arr[tree->_arr[x->p].right])) {
					//      then left[p[x]] = y
					tree->_arr[x->p].right = y->id;
				}
				//  else right[p[x]] = y
				else { tree->_arr[x->p].left = y->id; }
				// left[y] = x
				y->right = x->id;
				// p[x] = y
				x->p = y->id;
			}
			const RB_Node<Key, Data>* SEARCH(const RB_Node<Key, Data>* x, const Key& k) const
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &_arr[x->left];
					}
					else if (temp > 0) {
						x = &_arr[x->right];
					}
					else {
						break;
					}
				}

				return x;
			}



			RB_Node<Key, Data>* SEARCH(RB_Node<Key, Data>* x, const Key& k)
			{
				COMP3 comp3;
				//array_map_int count = 0;
				while (x->id) { // != nil
					//count++;
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &_arr[x->left];
					}
					else if (temp > 0) {
						x = &_arr[x->right];
					}
					else {
						break;
					}
				}
				//std::cout << count << "\n";
				return x;
			}



			const RB_Node<Key, Data>* SEARCH(const RB_Node<Key, Data>* x, Key&& k, Key* temp) const
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &_arr[x->left];
					}
					else if (temp > 0) {
						x = &_arr[x->right];
					}
					else {
						break;
					}
				}
				if (temp) {
					*temp = std::move(k);
				}
				return x;
			}
			RB_Node<Key, Data>* SEARCH(RB_Node<Key, Data>* x, Key&& k, Key* temp)
			{
				COMP comp;
				COMP3 comp3;

				while (!IsNULL(*x)) { // != nil
					int temp = comp3(k, x->first);
					if (temp < 0) { // k < x.key
						x = &_arr[x->left];
					}
					else if (temp > 0) {
						x = &_arr[x->right];
					}
					else {
						break;
					}
				}
				if (temp) {
					*temp = std::move(k);
				}
				return x;
			}
			void INSERT_FIXUP(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* z) /// google ..
			{
				RB_Node<Key, Data>* y;
				while (tree->_arr[z->p].color == RED) {
					if (z->p == tree->_arr[tree->_arr[z->p].p].left) {
						y = &(tree->_arr[tree->_arr[tree->_arr[z->p].p].right]);
						if (y->color == RED) {
							tree->_arr[z->p].color = BLACK;
							y->color = BLACK;
							tree->_arr[tree->_arr[z->p].p].color = RED;
							z = &(tree->_arr[tree->_arr[z->p].p]);
						}
						else
						{
							if (z == &(tree->_arr[tree->_arr[z->p].right])) {
								z = &(tree->_arr[z->p]);
								LEFT_ROTATE(tree, z);
							}
							tree->_arr[z->p].color = BLACK;
							tree->_arr[tree->_arr[z->p].p].color = RED;
							RIGHT_ROTATE(tree, &(tree->_arr[tree->_arr[z->p].p]));
						}
					}
					else {
						y = &(tree->_arr[_arr[tree->_arr[z->p].p].left]);
						if (y->color == RED) {
							tree->_arr[z->p].color = BLACK;
							y->color = BLACK;
							tree->_arr[tree->_arr[z->p].p].color = RED;
							z = &(tree->_arr[tree->_arr[z->p].p]);
						}
						else {
							if (z == &(tree->_arr[tree->_arr[z->p].left])) {
								z = &(tree->_arr[z->p]);
								RIGHT_ROTATE(tree, z);
							}
							tree->_arr[z->p].color = BLACK;
							tree->_arr[tree->_arr[z->p].p].color = RED;
							LEFT_ROTATE(tree, &(tree->_arr[tree->_arr[z->p].p]));
						}
					}
				}
				tree->_arr[tree->root].color = BLACK;
			}
			array_map_int INSERT(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key, array_map_int hint)
			{
				COMP comp;
				COMP2 eq;
				COMP3 comp3;

				array_map_int y_idx = 0;
				array_map_int x_idx = tree->root;
				auto& chk = tree->_arr;

				size_t now = tree->_arr.size(); //

				int select = 0; // 0 : nothing, 1: min, 2: max

				if (!IsNULL(tree->root)) {
					if (hint) {
						x_idx = hint;
					}
					else {
						if (tree->_arr[min_idx].first > key.first) {
							select = 1;
							hint = min_idx;
						}
						else if (tree->_arr[max_idx].first < key.first) {
							select = 2;
							hint = max_idx;
						}
					}

					if (hint) {
						x_idx = hint;
					}

					while (//!IsNULL(tree->arr[x_idx]) &&
						!IsNULL(x_idx) //&& !hint
						)
					{
						y_idx = x_idx;
						int test = comp3(key.first, tree->_arr[x_idx].first);

						if (test < 0)
						{
							x_idx = tree->_arr[x_idx].left;
						}
						else if (test > 0) {
							x_idx = tree->_arr[x_idx].right;
						}
						else {
							break;
						}
					}

					if (!IsNULL(x_idx) && eq(key.first, tree->_arr[x_idx].first)) {
						tree->_arr[x_idx].first = (key.first);
						tree->_arr[x_idx].second = (key.second);
						return x_idx;
					}
				}
				else {
					min_idx = now;
					max_idx = now;
				}

				RB_Node<Key, Data>* z = nullptr;
				if (0 == tree->dead_list) {
					tree->_arr.push_back(RB_Node<Key, Data>());
					tree->dead.push_back(0);
					tree->next.push_back(0);
					tree->_arr.back().id = now;
					tree->_arr.back().first = (key.first);
					tree->_arr.back().second = (key.second);

					z = &(tree->_arr.back());
				}
				else {
					now = tree->dead_list;
					tree->dead_list = tree->next[now];
					array_map_int id = tree->_arr[now].id;
					array_map_int next = tree->next[now];

					tree->_arr[now].Clear();
					tree->_arr[now].id = id;
					tree->_arr[now].first = (key.first);
					tree->_arr[now].second = (key.second);
					tree->next[now] = next;
					tree->dead[now] = false;

					z = &(tree->_arr[now]);
				}

				z->p = tree->_arr[y_idx].id;

				if (IsNULL(tree->_arr[y_idx])) {
					tree->root = z->id;
				}
				else if (comp(z->first, tree->_arr[y_idx].first)) { // comp3? 
					tree->_arr[y_idx].left = z->id;//
				}
				else {
					tree->_arr[y_idx].right = z->id;//
				}

				z->left = 0; // = nil
				z->right = 0;
				z->color = RED; // = RED

				// insert fixup
				INSERT_FIXUP(tree, z);

				count++;

				if (select == 1) {
					min_idx = z->id;
				}
				else if (select == 2) {
					max_idx = z->id;
				}
				return z->id;
			}
			array_map_int INSERT(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key, array_map_int hint)
			{

				COMP comp;
				COMP2 eq;
				COMP3 comp3;

				array_map_int y_idx = 0;
				array_map_int x_idx = tree->root;
				auto& chk = tree->_arr;

				size_t now = tree->_arr.size(); //

				int select = 0; // 0 : nothing, 1: min, 2: max

				if (!IsNULL(tree->root)) {
					if (hint) {
						x_idx = hint;
					}
					else {
						if (tree->_arr[min_idx].first > key.first) {
							select = 1;
							hint = min_idx;
						}
						else if (tree->_arr[max_idx].first < key.first) {
							select = 2;
							hint = max_idx;
						}
					}

					if (hint) {
						x_idx = hint;
					}

					while (//!IsNULL(tree->arr[x_idx]) &&
						!IsNULL(x_idx) //&& !hint
						)
					{
						y_idx = x_idx;
						int test = comp3(key.first, tree->_arr[x_idx].first);

						if (test < 0)
						{
							x_idx = tree->_arr[x_idx].left;
						}
						else if (test > 0) {
							x_idx = tree->_arr[x_idx].right;
						}
						else {
							break;
						}
					}

					if (!IsNULL(x_idx) && eq(key.first, tree->_arr[x_idx].first)) {
						tree->_arr[x_idx].first = std::move(key.first);
						tree->_arr[x_idx].second = std::move(key.second);
						return x_idx;
					}
				}
				else {
					min_idx = now;
					max_idx = now;
				}

				RB_Node<Key, Data>* z = nullptr;
				if (0 == tree->dead_list) {
					tree->_arr.push_back(RB_Node<Key, Data>());
					tree->dead.push_back(0);
					tree->next.push_back(0);
					tree->_arr.back().id = now;
					tree->_arr.back().first = std::move(key.first);
					tree->_arr.back().second = std::move(key.second);

					z = &(tree->_arr.back());
				}
				else {
					now = tree->dead_list;
					tree->dead_list = tree->next[now];
					array_map_int id = tree->_arr[now].id;
					array_map_int next = tree->next[now];

					tree->_arr[now].Clear();
					tree->_arr[now].id = id;
					tree->_arr[now].first = std::move(key.first);
					tree->_arr[now].second = std::move(key.second);
					tree->next[now] = next;
					tree->dead[now] = false;

					z = &(tree->_arr[now]);
				}

				z->p = tree->_arr[y_idx].id;

				if (IsNULL(tree->_arr[y_idx])) {
					tree->root = z->id;
				}
				else if (comp(z->first, tree->_arr[y_idx].first)) { // comp3? 
					tree->_arr[y_idx].left = z->id;//
				}
				else {
					tree->_arr[y_idx].right = z->id;//
				}

				z->left = 0; // = nil
				z->right = 0;
				z->color = RED; // = RED

				// insert fixup
				INSERT_FIXUP(tree, z);

				count++;

				if (select == 1) {
					min_idx = z->id;
				}
				else if (select == 2) {
					max_idx = z->id;
				}
				return z->id;
			}
			void LAZYINSERT(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key) {
				tree->remain_list.push_back(key);
			}
			void LAZYINSERT(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key) {
				tree->remain_list.push_back(std::move(key));
			}
			void PrintTree(RB_Tree<Key, Data, COMP>* tree) {
				if (tree->empty()) { return; }
				std::queue<int> que;

				que.push(tree->root);
				std::cout << "chk " << "\n";

				while (!que.empty()) {
					int id = que.front(); que.pop();

					std::cout << tree->_arr[id].p << " " << id << "\n";

					if (!IsNULL(tree->_arr[id].left)) {
						que.push(tree->_arr[id].left);
					}
					else {
						std::cout << " X ";
					}
					if (!IsNULL(tree->_arr[id].right)) {
						que.push(tree->_arr[id].right);
					}
					else {
						std::cout << "X ";
					}
				}
			}
		private:

			void _test(int64_t left, int64_t right, int64_t hint, RB_Tree<Key, Data, COMP>* tree, std::vector<std::pair<Key, Data>>* vec) {
				if (left > right) {
					return;
				}

				size_t middle = (left + right) / 2;

				hint = INSERT(tree, std::move((*vec)[middle]), hint);

				_test(left, middle - 1, hint, tree, vec);
				_test(middle + 1, right, hint, tree, vec);
			}
			// [left, right) ?
			void _test2(int64_t left, int64_t right, int64_t hint, RB_Tree<Key, Data, COMP>* tree, std::vector<std::pair<Key, Data>>* vec) {
				for (int64_t i = left; i < right; ++i) {
					INSERT(tree, std::move((*vec)[i]), 0);
				}
			}

			void REALINSERT(RB_Tree<Key, Data, COMP>* tree) {

				if (tree->remain_list.empty()) {
					return;
				}

				std::stable_sort(std::execution::par, tree->remain_list.begin(), tree->remain_list.end(), 
					[](const std::pair<Key, Data>& x, const std::pair<Key, Data>& y) { return x.first < y.first; });

				// remove dupplication? but no remove last dup?

				std::vector<std::pair<Key, Data>> vec;
				vec.reserve(tree->remain_list.size());

				bool first = true;
				std::pair<Key, Data> before;
				for (auto&& x : tree->remain_list) {
					if (!first && before.first == x.first) {
						continue;
					}
					before = x;
					first = false;
					vec.push_back(std::move(x));

					//INSERT(tree, std::move(x));
				}
				tree->remain_list.clear();

				// 
				if (first_time) {
					_test(0, vec.size() - 1, 0, tree, &vec);
				}
				else {
					_test2(0, vec.size(), 0, tree, &vec);
				}
				first_time = false;

				return;
			}

			RB_Node<Key, Data>* MAXIMUM(RB_Node<Key, Data>* x) {
				while (!IsNULL(x->right)) {
					x = &_arr[x->right];
				}
				return x;
			}
			RB_Node<Key, Data>* MINIMUM(RB_Node<Key, Data>* x)
			{
				while (!IsNULL(x->left)) { // != nil
					x = &_arr[x->left];
				}
				return x;
			}
			RB_Node<Key, Data>* SUCCESSOR(RB_Node<Key, Data>* x)
			{
				if (!IsNULL(x->right)) { // nil
					return MINIMUM(&_arr[x->right]);
				}

				RB_Node<Key, Data>* y = &_arr[x->p];
				while (!IsNULL(y->id) && x == &_arr[y->right])
				{
					x = y; y = &_arr[y->p];
				}
				return y;
			}

			void REMOVE_FIXUP(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* x)
			{
				RB_Node<Key, Data>* w;

				while (x->id != root && x->color == BLACK)
				{
					if (x == &(_arr[_arr[x->p].left])) {
						w = &(_arr[_arr[x->p].right]);
						if (w->color == RED) {
							w->color = BLACK;
							_arr[x->p].color = RED;
							LEFT_ROTATE(tree, &_arr[x->p]);
							w = &_arr[_arr[x->p].right];
						}
						if (_arr[w->left].color == BLACK && _arr[w->right].color == BLACK) {
							w->color = RED;
							x = &_arr[x->p];
						}
						else {
							if (_arr[w->right].color == BLACK) {
								_arr[w->left].color = BLACK;
								w->color = RED;
								RIGHT_ROTATE(tree, w);
								w = &_arr[_arr[x->p].right];
							}
							w->color = _arr[x->p].color;
							_arr[x->p].color = BLACK;
							_arr[w->right].color = BLACK;
							LEFT_ROTATE(tree, &_arr[x->p]);
							x = &_arr[root];
						}
					}
					else {
						w = &_arr[_arr[x->p].left];
						if (w->color == RED) {
							w->color = BLACK;
							_arr[x->p].color = RED;
							RIGHT_ROTATE(tree, &_arr[x->p]);
							w = &(_arr[_arr[x->p].left]);
						}
						if (_arr[w->left].color == BLACK && _arr[w->right].color == BLACK) {
							w->color = RED;
							x = &_arr[x->p];
						}
						else {
							if (_arr[w->left].color == BLACK) {
								_arr[w->right].color = BLACK;
								w->color = RED;
								LEFT_ROTATE(tree, w);
								w = &(_arr[_arr[x->p].left]);
							}
							w->color = _arr[x->p].color;
							_arr[x->p].color = BLACK;
							_arr[w->left].color = BLACK;
							RIGHT_ROTATE(tree, &_arr[x->p]);
							x = &_arr[root];
						}
					}
				}
				x->color = BLACK;
			}


			RB_Node<Key, Data>* REMOVE(RB_Tree<Key, Data, COMP>* tree, RB_Node<Key, Data>* z)
			{
				RB_Node<Key, Data>* x;
				RB_Node<Key, Data>* y;

				if (IsNULL(z->left)
					|| IsNULL(z->right)) {
					y = z;
				}
				else { y = SUCCESSOR(z); }

				if (!IsNULL(y->left)) {
					x = &(tree->_arr[y->left]);
				}
				else { x = &(tree->_arr[y->right]); }

				x->p = y->p;

				if (IsNULL(y->p)) {
					tree->root = x->id;
				}
				else if (y == &(_arr[_arr[y->p].left])) {
					_arr[y->p].left = x->id;
				}
				else { _arr[y->p].right = x->id; }

				if (y != z) { //important part!
					z->first = std::move(y->first); // chk??
					z->second = std::move(y->second);
					std::swap(z->dead, y->dead);
					std::swap(z->next, y->next);
				}
				if (y->color == BLACK) {
					REMOVE_FIXUP(tree, x);
				}
				return y;
			}
		public:
			void PrintTree() {
				PrintTree(this);
			}
			void LazyInsert(RB_Tree<Key, Data, COMP>* tree, const std::pair<Key, Data>& key) {
				LAZYINSERT(this, key);
			}
			void LazyInsert(RB_Tree<Key, Data, COMP>* tree, std::pair<Key, Data>&& key) {
				LAZYINSERT(this, std::move(key));
			}
			// insert, search, remove.
			array_map_int Insert(const std::pair<Key, Data>& key)
			{
				return INSERT(this, key, 0);
			}
			array_map_int Insert(std::pair<Key, Data>&& key)
			{
				return INSERT(this, key, 0);
			}

			bool IsExist(const Key& key) const
			{
				// NULL != ~
				return !IsNULL(*SEARCH(&_arr[root], key));
			}
			bool IsExist(Key&& key) const
			{
				// NULL != ~
				return !IsNULL(*SEARCH(&_arr[root], key));
			}

			RB_Node<Key, Data>* Search(const Key& key, array_map_int* id = nullptr) {
				RB_Node<Key, Data>* x = &_arr[root];

				x = SEARCH(x, key);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			const RB_Node<Key, Data>* Search(const Key& key, array_map_int* id = nullptr) const {
				const RB_Node<Key, Data>* x = &_arr[root];

				x = SEARCH(x, key);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			RB_Node<Key, Data>* Search(Key&& key, array_map_int* id = nullptr, Key* temp = nullptr) {
				RB_Node<Key, Data>* x = SEARCH(&_arr[root], std::move(key), temp);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			const RB_Node<Key, Data>* Search(Key&& key, array_map_int* id = nullptr, Key* temp = nullptr) const {
				const RB_Node<Key, Data>* x = SEARCH(&_arr[root], std::move(key), temp);

				if (nullptr != id) {
					*id = x->id;
				}

				return x;
			}

			void RealInsert()
			{
				REALINSERT(this);
			}

			void Remove(const Key& key)
			{
				RB_Node<Key, Data>* node = SEARCH(&_arr[root], key);

				if (!IsNULL(*node))
				{

					RB_Node<Key, Data>* temp = REMOVE(this, node);

					temp->dead = true;
					temp->next = this->dead_list;
					this->dead_list = temp->id;
					count--;
				}
			}

			void Remove(Key&& key)
			{
				RB_Node<Key, Data>* node = SEARCH(&_arr[root], std::move(key));

				if (!IsNULL(*node))
				{
					RB_Node<Key, Data>* temp = REMOVE(this, node);

					temp->dead = true;
					temp->next = this->dead_list;
					this->dead_list = temp->id;
					count--;
				}
			}

			Data& DataVal(array_map_int idx) {
				return _arr[idx].second;
			}
			const Data& DataVal(array_map_int idx) const {
				return _arr[idx].second;
			}

			bool IsEmpty() const
			{
				return 0 == count;
			}
			bool empty() const { return IsEmpty(); }
			array_map_int GetCount() const
			{
				return count;
			}
			array_map_int size() const { return count; }
			void clear() {
				Clear();
			}
		};

	public:
		using iterator = typename std::vector<RB_Node<Key, Data>>::iterator;
		using const_iterator = typename std::vector<RB_Node<Key, Data>>::const_iterator;
	private:
		RB_Tree<Key, Data>* arr = nullptr;
		Allocator<RB_Node<Key, Data>> allocator;
	private:
		array_map() { 
			//
		}
	public:
		explicit array_map(Arena& pool, size_t reserve_num = 0) : allocator(pool) {
			arr = new RB_Tree<Key, Data>(allocator);
			if (reserve_num > 0) {
				arr->reserve(reserve_num);
			}
		}
		~array_map() {
			if (arr) {
				delete arr;
			}
		}
	public:
		void PrintTree() {
			arr->PrintTree();
		}
		bool empty() const {
			arr->RealInsert();

			return arr->empty();
		}
		auto size() const {
			arr->RealInsert();

			return arr->size();
		}
		void clear() {
			arr->clear();
		}
		void reserve(array_map_int x) {
			arr->reserve(x);
		}
		void reserve2(array_map_int x) {
			arr->reserve_remain(x);
		}
	public:
		iterator begin() {
			arr->RealInsert();

			return arr->begin();
		}
		iterator end() {
			arr->RealInsert();

			return arr->end();
		}
		const_iterator begin() const {
			arr->RealInsert();

			return arr->begin();
		}
		const_iterator end() const {
			arr->RealInsert();

			return arr->end();
		}
		iterator find(const Key& key) {
			arr->RealInsert();

			array_map_int id;
			RB_Node<Key, Data>* x = arr->Search(key, &id);
			if (0 == x->id) {
				return arr->end();
			}

			auto result = begin() + (x->id - 1);

			return result;
		}
		const_iterator find(const Key& key) const {
			arr->RealInsert();

			array_map_int id;
			const RB_Node<Key, Data>* x = arr->Search(key, &id);
			if (0 == x->id) {
				return arr->end();
			}
			return arr->begin() + (x->id - 1);
		}
		iterator find(Key&& key) {
			arr->RealInsert();

			array_map_int id;
			RB_Node<Key, Data>* x = arr->Search(std::move(key), &id);
			if (0 == x->id) {
				return arr->end();
			}
			return begin() + (x->id - 1);
		}
		const_iterator find(Key&& key) const {
			arr->RealInsert();

			array_map_int id;
			const RB_Node<Key, Data>* x = arr->Search(std::move(key), &id);
			if (0 == x->id) {
				return arr->end();
			}
			return arr->begin() + (x->id - 1);
		}

	public:
		void insert(const std::pair<Key, Data>& value) {
			lazy_insert(value);

			//	arr->RealInsert();

			//	arr->Insert(wiz::Pair<Key, Data>(value.first, value.second));
		}
		void insert(std::pair<Key, Data>&& value) {
			lazy_insert(value);

			//arr->RealInsert();

		//	arr->Insert(wiz::Pair<Key, Data>(value.first, value.second));
		}
		void update()
		{
			arr->RealInsert();
		}
		void lazy_insert(const std::pair<Key, Data>& value) {
			arr->LazyInsert(arr, value);
		}
		void lazy_insert(std::pair<Key, Data>&& value) {
			arr->LazyInsert(arr, std::move(value));
		}

		void remove(const std::pair<Key, Data>& value)
		{
			arr->RealInsert();

			arr->Remove(value.first);
		}
		void remove(std::pair<Key, Data>&& value)
		{
			arr->RealInsert();

			arr->Remove(std::move(value.first));
		}
		Data& at(const Key& key) {
			return (*this)[key];
		}
		const Data& at(const Key& key) const {
			return find(key)->second;
		}
		Data& at(Key&& key) {
			return (*this)[std::move(key)];
		}
		const Data& at(Key&& key) const {
			return find(std::move(key))->second;
		}

		Data& operator[](const Key& key) {
			arr->RealInsert();
			
			RB_Node<Key, Data>* idx = arr->Search(key); // const_cast<RB_Node<Key, Data>*>(arr->Search(key));

			if (0 == idx->id) {
				array_map_int _idx = arr->Insert(std::pair<Key, Data>(key, Data())); //// return position? - to do
				return arr->DataVal(_idx);
			}
			else {
				return idx->second;
			}
		}

		const Data& operator[](const Key& key) const {
			arr->RealInsert();

			const RB_Node<Key, Data>* idx = arr->Search(key);
			if (0 == idx->id) {
				//array_map_int _idx = arr->Insert(wiz::Pair<Key, Data>(key, Data())); //// return positon? - to do
				//return arr->DataVal(_idx);
				throw "do not exist in array_map";
			}
			else {
				return idx->second;
			}
		}

		Data& operator[](Key&& key) {
			arr->RealInsert();
			Key temp;

			RB_Node<Key, Data>* idx = arr->Search(std::move(key), nullptr, &temp);
			if (0 == idx->id) {
				array_map_int _idx = arr->Insert(std::pair<Key, Data>(std::move(temp), Data())); //// return positon? - to do
				return arr->DataVal(_idx);
			}
			else {
				return idx->second;
			}
		}

		const Data& operator[](Key&& key) const {
			arr->RealInsert();

			const RB_Node<Key, Data>* idx = arr->Search(std::move(key));
			if (0 == idx->id) {
				//array_map_int _idx = arr->Insert(wiz::Pair<Key, Data>(std::move(key), Data())); //// return positon? - to do
				//return arr->Idx(_idx).second;
				throw "do not exist in array_map";
			}
			else {
				return idx->second;
			}
		}

	};
}

#endif
#pragma once
