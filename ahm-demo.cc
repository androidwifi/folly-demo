#include <thread>
#include <memory>
#include <mutex>

#include <folly/AtomicHashMap.h>
#include <folly/ScopeGuard.h>
#include <folly/Memory.h>

namespace {
	struct MyObject {
		explicit MyObject(int i) : i(i) {}
		int i;
	};

	typedef folly::AtomicHashMap<int, std::shared_ptr<MyObject>> MyMap;
	typedef std::lock_guard<std::mutex> Guard;

	std::unique_ptr<MyMap> newMap() {
		return folly::make_unique<MyMap>(100);
	}

	struct MyObjectDirectory {
		MyObjectDirectory() : cur_(newMap()), prev_(newMap()) {}

		std::shared_ptr<MyObject> get(int key) {
			auto val = tryGet(key);
			if (val)
				return val;

			std::shared_ptr<MyMap> cur;
			{
				Guard g(lock_);
				cur = cur_;
			}

			auto ret = cur->insert(key, std::make_shared<MyObject>(key));
			return ret.first->second; // ret.first 指向一个 map node，node->second 为 key/val 中的 val
		}

		std::shared_ptr<MyObject> tryGet(int key) {
			std::shared_ptr<MyMap> cur;
			std::shared_ptr<MyMap> prev;
			{
				Guard g(lock_);
				cur = cur_;
				prev = prev_;
			}

			auto it = cur->find(key);
			if (it != cur->end())
				return it->second;

			// 此处是何意?
			it = prev->find(key);
			if (it != prev->end()) {
				auto ret = cur->insert(key, it->second);
				return ret.first->second;
			}
			return nullptr;
		}

		void archive() {
			std::shared_ptr<MyMap> cur(newMap());
			Guard g(lock_);
			prev_ = cur_;
			cur_ = cur;
		}

		std::mutex lock_;

		// 此处是 2 个 map，在后面的线程例子中，cur_ 和 prev_ 会经常变更：
		//   先在 cur_ 中查找元素，如果没有，则插入被查找的元素
		//
		//   archive 操作实际上是将 cur_ 替换掉 prev_，即丢弃之前的 prev_，然后
		//   cur_ 被重置。
		std::shared_ptr<MyMap> cur_;
		std::shared_ptr<MyMap> prev_;
	};
}

int main(int argc, char *argv[]) {
	auto const objs = new MyObjectDirectory();
	SCOPE_EXIT { delete objs; };

	std::vector<std::thread> threads;
	for (int threadId =0; threadId < 512; ++threadId) {
		threads.emplace_back(
			//[objs, threadId] {
			[objs] {
				for (int recycles = 0; recycles < 5000; ++recycles) {
					for (int i= 0; i < 1000; i++) {
						auto val = objs->get(i);
					}

					objs->archive();
				}
			}
		);
	}

	for (auto& t : threads)
		t.join();
}
