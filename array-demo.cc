#include <folly/Array.h>
#include <folly/portability/GTest.h>
#include <string>
#include <iostream>

int main() {
	auto arr = folly::make_array<int>();
	static_assert(std::is_same<typename decltype(arr)::value_type, int>::value, "Wrong array type");
	EXPECT_EQ(arr.size(), 0);
	std::cout << "OK" << std::endl;
}
