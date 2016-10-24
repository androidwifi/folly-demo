#include <folly/Foreach.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <map>

void foreach_rvalue() {
	const char * const hello = "hello";
	int n = 0;

	// 正向迭代
	FOR_EACH(it, std::string(hello)) {
		++n;
	}

	assert(n == strlen(hello));

	// 反向(reverse)迭代
	FOR_EACH_R(it, std::string(hello)) {
		--n;
		assert(hello[n] == *it);
		std::cout << *it << std::endl;
	}

	assert(n == 0);
}

void foreach_kv() {
	std::map<std::string, int> map__;
	map__["abc"] = 1;
	map__["def"] = 2;
	std::string keys = "";
	int values = 0;
	int nr_entries = 0;
	FOR_EACH_KV(key, value, map__) {
		keys += key;
		values += value;
		++nr_entries;
	}
	assert("abcdef" == keys);
	assert(3 == values);
	assert(2 == nr_entries);
}

void foreach_kv_multimap() {
	std::multimap<std::string, int> map__;
	map__.insert(std::make_pair("abc", 1));
	map__.insert(std::make_pair("abc", 2));
	map__.insert(std::make_pair("def", 3));
	std::string keys = "";
	int values = 0;
	int nr_entries = 0;
	FOR_EACH_KV(k, v, map__) {
		keys += k;
		values += v;
		++nr_entries;
	}
	
	assert("abcabcdef" == keys);
	assert(6 == values);
	assert(3 == nr_entries);
}

int main() {
	foreach_rvalue();
	foreach_kv();
	foreach_kv_multimap();
}
