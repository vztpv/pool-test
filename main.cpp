#include <iostream>
#include <vector>
#include <string>
#include "array_map.h"

template <class Block, class Len>
class Header {
public:
	union {
		Block* block;
		Len len;
	};
};

template <class Block>
class BlockManager { // manager for not using?
public: // or friend? or set_~~ 
	// start_block-> ...->last_block(or nullptr)
	Block* start_block = nullptr;
	Block* last_block = nullptr;
public:
	[[nodiscard]]
	Block* Get(uint64_t cap) {
		if (!start_block) {
			start_block = new (std::nothrow)Block(cap);
			if (start_block == nullptr) { return nullptr; }
			Block* result = start_block;
			start_block = nullptr;
			result->state = 1;
			result->free_next = nullptr;
			return result;
		}
		Block* block = start_block;
		Block* before_block = nullptr;

		while (block && (block->capacity != cap)) {
			before_block = block;
			block = block->next;
		}
		if (block) {
			if (before_block) {
				before_block->next = block->next;
			}
			else { // 
				start_block = block->next;
				if (start_block == nullptr) {
					last_block = nullptr;
				}
			}
			block->offset = 0;
			block->next = nullptr;
			block->state = 1;
			block->free_next = nullptr;
			return block;
		}
		return new(std::nothrow)Block(cap);
	}
public:
	BlockManager(Block* start_block = nullptr, Block* last_block = nullptr) : start_block(start_block), last_block(last_block) {
		if (last_block) {
			last_block->next = nullptr;
		}
	}

	// check last_block?, has bug?
	void RemoveBlocks() {
		//t64_t count = 0;
		Block* block = start_block;
		while (block) {
			Block* next = block->next;
			delete block;
			block = next;
			//unt++;
		}	//d::cout << "count1 " << count << " \n";
	}
};

// bug - 크기를 줄일떄? 메모리 소비?
// memory_pool?
class Arena {
public:
	struct Block {
		Block* next; //
		Block* free_next;
		uint64_t capacity;
		uint64_t offset;
		uint64_t count;
		uint64_t state; // 0 free, 1 used
		uint8_t* data;

		Block(uint64_t cap)
			: next(nullptr), free_next(nullptr), capacity(cap), offset(0), count(0), state(1) {
			//data = (uint8_t*)mi_malloc(sizeof(uint8_t) * capacity); // 
			data = new (std::nothrow) uint8_t[capacity];
		}

		~Block() {
			delete[] data;
			data = nullptr;
			//	mi_free(data);
		}

		Block(const Block&) = delete;
		Block& operator=(const Block&) = delete;
	};
public:
	Block* free_block[2];
	Block* head[2];
	Block* rear[2];
	const uint64_t defaultBlockSize;
	uint64_t count = 0;
public:
	static const uint64_t initialSize = 1024 * 1024; // 4 * 1024?

private:
	BlockManager<Block> blockManager[2];
	std::vector<Block*> startBlockVec[2];
	std::vector<Block*> lastBlockVec[2];

private:
	void RemoveBlocks(int no) {
		//
		Clear(no);
		blockManager[no].RemoveBlocks();
		//
		Block* block = head[no];
		//t count = 0;
		while (block) {
			Block* next = block->next;
			delete block;
			block = next;
			//unt++;
		}
		//d::cout << "count2 " << count << " \n";
	}

	void Clear(int no) {
		if (lastBlockVec[no].empty()) {
			if (blockManager[no].last_block) {
				blockManager[no].last_block->next = head[no];
			}
			else {
				blockManager[no].start_block = head[no];
			}
			if (!blockManager[no].start_block) {
				blockManager[no].start_block = head[no];
			}

			blockManager[no].last_block = rear[no];
			if (blockManager[no].last_block) {
				blockManager[no].last_block->next = nullptr;
			}

			head[no] = blockManager[no].Get(defaultBlockSize);
			rear[no] = head[no];
		}
		else {
			Reset(no);
		}
	}
private:
	// link_from -> Reset -> DivideBlock -> link_from...
	void Reset(int no) {
		if (lastBlockVec[no].empty()) {
			return;
		}

		if (blockManager[no].last_block) {
			blockManager[no].last_block->next = head[no];
		}
		else {
			blockManager[no].start_block = head[no];
		}
		if (!blockManager[no].start_block) {
			blockManager[no].start_block = head[no];
		}

		blockManager[no].last_block = lastBlockVec[no][0];
		if (blockManager[no].last_block) {
			blockManager[no].last_block->next = nullptr;
		}

		head[no] = blockManager[no].Get(defaultBlockSize);
		rear[no] = head[no];
	}

public:
	void RemoveBlocks() {
		RemoveBlocks(0);
		RemoveBlocks(1);
	}
	void Clear() {
		Clear(0);
		Clear(1);
	}

public:
	Arena(uint64_t size = initialSize) : defaultBlockSize(size) {
		for (int i = 0; i < 2; ++i) { // i == 0 4k, i == 1  > 4k
			blockManager[i] = BlockManager<Block>(nullptr, nullptr);
			head[i] = blockManager[i].Get(defaultBlockSize);
			rear[i] = head[i];
			free_block[i] = nullptr;
		}
	}
	/*
	Arena(Block* start_block0, Block* last_block0, Block* start_block1, Block* last_block1, uint64_t size = initialSize)
		: defaultBlockSize(size) {
		blockManager[0] = BlockManager<Block>(start_block0, last_block0);
		blockManager[1] = BlockManager<Block>(start_block1, last_block1);

		for (int i = 0; i < 2; ++i) { // i < 2
			head[i] = blockManager[i].Get(defaultBlockSize); // (new (std::nothrow) Block(initialSize));
			rear[i] = head[i];
			free_block[i] = nullptr;
		}
	}
	*/
	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;

public:
	template <class T>
	T* allocate(uint64_t size) {
		return _allocate<T>(size * sizeof(T) + sizeof(Header<Block, uint64_t>) * 2);
	}
private:
	template <class T>
	T* _allocate(uint64_t size, uint64_t align = alignof(T)) {
	
		int no = 0;
		if (size + 64 >= defaultBlockSize) {
			no = 1;
		}

		do {
			if (free_block[no]) {
				Block* block = free_block[no];
				if (block) {
					if (block->offset + size < block->capacity) {
						uint64_t remain = block->capacity - block->offset;

						void* ptr = block->data + block->offset;
						void* aligned_ptr = ptr;

						if (std::align(alignof(T), size, aligned_ptr, remain)) {
							size_t aligned_offset = static_cast<uint8_t*>(aligned_ptr) - block->data;

							block->offset = aligned_offset + size;
							block->count++;
							Header<Block, uint64_t>* pheader = static_cast<Header<Block, uint64_t>*>(aligned_ptr);
							pheader->block = block;
							pheader = pheader + 1;
							pheader->len = size - sizeof(Header<Block, uint64_t>) * 2;
							pheader = pheader + 1;

							block->state = 1;
							block->free_next = nullptr;

							free_block[no] = free_block[no]->free_next;
							return reinterpret_cast<T*>(pheader);
						}
					}
				}
				if (no == 1) {
					break;
				}
			}

			Block* block = this->head[no];

			if (block) {
				if (block->offset + size < block->capacity) {
					uint64_t remain = block->capacity - block->offset;

					void* ptr = block->data + block->offset;
					void* aligned_ptr = ptr;

					if (std::align(alignof(T), size, aligned_ptr, remain)) {
						size_t aligned_offset = static_cast<uint8_t*>(aligned_ptr) - block->data;

						block->offset = aligned_offset + size;
						block->count++;
						Header<Block, uint64_t>* pheader = static_cast<Header<Block, uint64_t>*>(aligned_ptr);
						pheader->block = block;
						pheader = pheader + 1;
						pheader->len = size - sizeof(Header<Block, uint64_t>) * 2;
						uint64_t length = pheader->len;
						pheader = pheader + 1;
						return reinterpret_cast<T*>(pheader);
					}
				}
				block = block->next;
			}
		} while (false);

		{
			// allocate new block
			uint64_t newCap = std::max(defaultBlockSize, size + 64);
			int no = 0;
			if (newCap == size + 64) {
				no = 1;
			}
			Block* newBlock = blockManager[no].Get(newCap); // new (std::nothrow) Block(newCap);
			if (!newBlock) {
				return nullptr;
			}

			uint64_t remain = newBlock->capacity - newBlock->offset;
			void* ptr = newBlock->data + newBlock->offset;
			void* aligned_ptr = ptr;

			if (std::align(alignof(T), size, aligned_ptr, remain)) {
				uint64_t aligned_offset = static_cast<uint8_t*>(aligned_ptr) - newBlock->data;

				newBlock->offset = aligned_offset + size;

				newBlock->next = this->head[no];

				this->head[no] = newBlock;
				newBlock->count++;
				Header<Block, uint64_t>* pheader = static_cast<Header<Block, uint64_t>*>(aligned_ptr);
				pheader->block = newBlock;
				pheader = pheader + 1;
				pheader->len = size - sizeof(Header<Block, uint64_t>) * 2;
				pheader = pheader + 1;
				return reinterpret_cast<T*>(pheader);
			}
		}
		return nullptr;
	}
public:
	// expand
	template <class T>
	void deallocate(T* ptr) {

		//return;

		Header<Block, uint64_t>* p = reinterpret_cast<Header<Block, uint64_t>*>(ptr);
		p = p - 1;
		uint64_t len = p->len;
		p = p - 1;
		Block* block = p->block;

		block->count--;
		if (block->count != 0) {
			if (reinterpret_cast<uint8_t*>(ptr) + sizeof(T) * len == static_cast<uint8_t*>(block->data) + block->offset) {
				block->offset = reinterpret_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(block->data);
				block->offset = block->offset - sizeof(Header<Block, uint64_t>) * 2;
			}
		}
		else {
			block->state = 0;
			block->offset = 0;
			block->next = nullptr;

			if (len + 64 >= defaultBlockSize) {
				block->free_next = free_block[1];
				free_block[1] = block;
			}
			else {
				block->free_next = free_block[0];
				free_block[0] = block;
			}
		}

		return;

		//return;

		// chk !
		/*Block* block = this->head[0];

		while (block) {
			if ((uint8_t*)(ptr)+sizeof(T) * len == (uint8_t*)(block->data) + block->offset) {
				block->offset = (uint8_t*)ptr - (uint8_t*)block->data;
				//std::cout << "real_deallocated\n"; //
				return;
			}

			block = block->next;
		}
		*/
	}

	template<typename T, typename... Args>
	T* create(Args&&... args) {
		void* mem = allocate<T>(sizeof(T), alignof(T));
		return new (mem) T(std::forward<Args>(args)...);
	}

public:
	~Arena() {
		RemoveBlocks();
	}
};


template<typename T>
class ArenaAllocator {
public:
	using value_type = T;

	Arena* arena_ = nullptr;

	ArenaAllocator() {
		//
	}
	explicit ArenaAllocator(Arena& arena)
		: arena_(&arena) {
	}

	template<typename U>
	ArenaAllocator(const ArenaAllocator<U>& other)
		: arena_(other.arena_) {
	}

	T* allocate(std::size_t n) {
		return arena_->allocate<T>(n);
	}

	void deallocate(T* ptr, std::size_t) noexcept {
		arena_->deallocate<T>(ptr);
	}

	template<typename U>
	bool operator==(const ArenaAllocator<U>& other) const {
		return arena_ == other.arena_;
	}

	template<typename U>
	bool operator!=(const ArenaAllocator<U>& other) const {
		return !(*this == other);
	}
};


#include <ctime>
#include <set>
#include <map>
#include <cstdlib>

int main(void)
{
	const int MAX = 1024000;
	const int TIMES = 4;
	srand(0);
	std::vector<int> rand_vec(MAX);
	for (int i = 0; i < MAX; ++i) {
		rand_vec[i] = rand() * rand() * rand();
	}

	{
		int a, b;
		for (int i = 0; i < TIMES; ++i) {
			a = clock();
			std::map<int, int> x;
			for (int i = 0; i < MAX; ++i) {
				x.insert({ rand_vec[i], i });
			}
			b = clock();
			std::cout << x.size() << " ";
			std::cout << b - a << "ms\n";
			int sum = 0;
			for (int i = 0; i < MAX; ++i) {
				sum += x[rand_vec[i]];
			}
			std::cout << clock() - b << "ms\t";
			std::cout << sum << "\n";
		}
	}
	std::cout << "--------------\n";
	
	{
		int a, b;
		
		Arena pool;
		for (int i = 0; i < TIMES; ++i) {
			a = clock();
			std::map<int, int, std::less<int>, ArenaAllocator<std::pair<const int, int>>> y{ ArenaAllocator<std::pair<const int, int>>(pool) };
			for (int i = 0; i < MAX; ++i) {
				y.insert({ rand_vec[i], i});
			}
			uint64_t sz = y.size();
			pool.Clear();
			b = clock();
			std::cout << sz << " ";
			std::cout << b - a << "ms\n";

			int sum = 0;
			for (int i = 0; i < MAX; ++i) {
				sum += y[rand_vec[i]];
			}
			std::cout << clock() - b << "ms\t";
			std::cout << sum << "\n";
		}
	}		

	std::cout << "--------------\n";

	// array_map
	{
		int a, b;
		for (int i = 0; i < TIMES; ++i) {
			a = clock();
			wiz::array_map<int, int> z;
			for (int i = 0; i < MAX; ++i) {
				z.insert({ rand_vec[i], i });
			}
			int sz = z.size();
			b = clock();
			std::cout << sz << " ";
			std::cout << b - a << "ms\n";

			int sum = 0;
			for (int i = 0; i < MAX; ++i) {
				sum += z[rand_vec[i]];
			}
			std::cout << clock() - b << "ms\t";
			std::cout << sum << "\n";
		}
	}

	std::cout << "--------------\n";

	// array_map + arena.
	{
		int a, b;
		Arena pool;
		for (int i = 0; i < TIMES; ++i) {
			a = clock();
			clau::array_map<int, int, Arena, ArenaAllocator> w(pool);
			for (int i = 0; i < MAX; ++i) {
				w.insert({ rand_vec[i], i });
			}
			int sz = w.size();
			pool.Clear();
			b = clock(); 
			std::cout << sz << " ";
			std::cout << b - a << "ms\n";

			int sum = 0;
			for (int i = 0; i < MAX; ++i) {
				sum += w[rand_vec[i]];
			}
			std::cout << clock() - b << "ms\t";
			std::cout << sum << "\n";
		}
	}
	
	std::cout << "ends\n";

	return 0;
}
