# Intro

A folly demo based on existed folly test code

# Usage

* Download/clone folly original code and build/install them
	* Usually installed to `~/folly`
	* build `gtest-1.7.0`, copy to ~/gtest-1.7.0 (gtest not allowe `make install`)

* Update `~/.bashrc`, add following two export(assume current home is `/root`)
	* `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/folly/lib`
	* `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/gtest-1.7.0/lib/.libs/`
	* update: `source ~/.bashrc`

* Install `scons`, its a make file replacement `apt-get install scons`

* Recomment install `clang++` for better build output

# Example

	// array-demo.cc
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

Build:

	$ ./b.sh array-demo.cc
	scons: Reading SConscript files ...
	scons: done reading SConscript files.
	scons: Building targets ...
	clang++-3.5 -o array-demo.o -c -std=c++11 -g -O0 -Wall -static -I/usr/include -I/root/folly/include -I/root/gtest-1.7.0/include array-demo.cc
	clang++-3.5 -o a.out array-demo.o -L/root/gtest-1.7.0/lib/.libs -L/root/folly/lib -lgtest -lfolly
	scons: done building targets.

Test linking:

	$ ldd a.out
	linux-vdso.so.1 =>  (0x00007ffcd6f6c000)
	libgtest.so.0 => /root/gtest-1.7.0/lib/.libs/libgtest.so.0 (0x00007f7093dcf000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f7093ba6000)
	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f70938a1000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f709368b000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f70932c6000)
	/lib64/ld-linux-x86-64.so.2 (0x000056101ea96000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f7092fbf000)

Run:

	$ ./a.out
	OK
