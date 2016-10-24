#include <folly/Baton.h>
#include <folly/test/BatonTestHelpers.h>
#include <folly/test/DeterministicSchedule.h>
#include <thread>
#include <semaphore.h>
#include <gtest/gtest.h>

using namespace folly;
using namespace folly::test;
using folly::detail::EmulatedFutexAtomic;

void basic() {
	Baton<> b;
	b.post();
	b.wait();
}

void timed_wait_basic_system_clock() {
	run_basic_timed_wait_tests<std::atomic, std::chrono::system_clock>();
	run_basic_timed_wait_tests<EmulatedFutexAtomic, std::chrono::system_clock>();
	run_basic_timed_wait_tests<DeterministicAtomic, std::chrono::system_clock>();
}
 
int main() {
	basic();
	timed_wait_basic_system_clock();
}
