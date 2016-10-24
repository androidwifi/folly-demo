#include <folly/Arena.h>
#include <folly/Memory.h>

#include <set>
#include <vector>
#include <cassert>
#include <iostream>
#include <glog/logging.h>

using namespace folly;
//static_assert(IsArenaAllocator<SysArena>::value, "");

void size_sanity() {
	std::set<size_t*> allocated_items;
	static const size_t requested_block_size = 64;
	SysArena arena(requested_block_size);
	size_t minimum_size = sizeof(SysArena), maximum_size = minimum_size;

	assert(arena.totalSize() == minimum_size);

	// 插入一个元素，并获取一个新的 block
	size_t *ptr = static_cast<size_t*>(arena.allocate(sizeof(long)));
	allocated_items.insert(ptr);
	minimum_size += requested_block_size;
	maximum_size += goodMallocSize(requested_block_size + SysArena::kBlockOverhead);
	assert(arena.totalSize() >= minimum_size);
	assert(arena.totalSize() <= maximum_size);
	std::cout << minimum_size << " < " << arena.totalSize() << " < " << maximum_size << std::endl;

	// 插入一个更大的元素, size 保持不变
	ptr = static_cast<size_t*>(arena.allocate(requested_block_size/2));
	allocated_items.insert(ptr);
	assert(arena.totalSize() >= minimum_size);
	assert(arena.totalSize() <= maximum_size);
	std::cout << minimum_size << " < " << arena.totalSize() << " < " << maximum_size << std::endl;

	// 插入 10 个 block
	for (int i=0; i<10; i++) {
		ptr = static_cast<size_t*>(arena.allocate(requested_block_size));
		allocated_items.insert(ptr);
	}
	minimum_size += 10*requested_block_size;
	maximum_size += 10*goodMallocSize(requested_block_size + SysArena::kBlockOverhead);
	assert(arena.totalSize() >= minimum_size);
	assert(arena.totalSize() <= maximum_size);
	std::cout << minimum_size << " < " << arena.totalSize() << " < " << maximum_size << std::endl;

	// 插入巨型对象
	ptr = static_cast<size_t*>(arena.allocate(10*requested_block_size));
	allocated_items.insert(ptr);
	minimum_size += 10*requested_block_size;
	maximum_size += goodMallocSize(10*requested_block_size + SysArena::kBlockOverhead);
	assert(arena.totalSize() >= minimum_size);
	assert(arena.totalSize() <= maximum_size);
	std::cout << minimum_size << " < " << arena.totalSize() << " < " << maximum_size << std::endl;

	// 释放对象
	for (const auto& item : allocated_items) {
		arena.deallocate(item);
	}
	// 但 size 保持不变
	assert(arena.totalSize() >= minimum_size);
	assert(arena.totalSize() <= maximum_size);
	std::cout << minimum_size << " < " << arena.totalSize() << " < " << maximum_size << std::endl;
}

int main() {
	size_sanity();
}
