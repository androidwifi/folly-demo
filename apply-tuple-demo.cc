#include <iostream>
#include <gtest/gtest.h>
#include <folly/ApplyTuple.h>

struct Mover {
	Mover() {}
	Mover(Mover&&) noexcept {}
	Mover(const Mover&&) = delete;
	Mover& operator=(const Mover&) = delete;
};

void move_only_func(Mover&&) {}

struct Overloaded {
	int func(int) { return 0; }
	bool func(bool) { return true; }
};

struct ConstOverloaded {
	ConstOverloaded() {}
	int operator()() { return 101; }
	int operator()() const { return 102; }
};

void func(int a, int b, double c) {
	EXPECT_EQ(a, 1);
	EXPECT_EQ(b, 2);
	EXPECT_EQ(c, 3.0);
}

std::function<void(int, int, double)> makeFunc() {
	return &func;
}

struct Wat {
	void func(int a, int b, double c) {
		::func(a,b,c);
	}

	double retVal(int a, int b) {
		return a+b;
	}

	Wat() {}
	Wat(Wat const&) = delete;
	int foo;
};

struct GuardObjBase {
	GuardObjBase(GuardObjBase&&) noexcept {}
	GuardObjBase() {}
	GuardObjBase(GuardObjBase const &) = delete;
	GuardObjBase& operator=(GuardObjBase const&) = delete;
};
typedef GuardObjBase const& Guard;

template<class F, class Tuple>
struct GuardObj : GuardObjBase {
	explicit GuardObj(F&& f, Tuple&& args) :
		f_(std::forward<F>(f)),
		args_(std::forward<Tuple>(args)){}

	GuardObj(GuardObj&& g) noexcept :
		GuardObjBase(std::move(g)),
		f_(std::move(g.f_)),
		args_(std::move(g.args_)){}

	~GuardObj() {
		folly::applyTuple(f_, args_);
	}

	GuardObj(const GuardObj&) = delete;
	GuardObj& operator=(const GuardObj&) = delete;

private:
	F f_;
	Tuple args_;
};

template<class F, class ...Args>
GuardObj<typename std::decay<F>::type, std::tuple<Args...>>
guard(F&& f, Args&&... args) {
	return GuardObj<typename std::decay<F>::type, std::tuple<Args...>>
		(std::forward<F>(f), std::tuple<Args...>(std::forward<Args>(args)...));
}

struct CopyCount {
	CopyCount() {}
	CopyCount(CopyCount const&) {
		std::cout << "copy count copy ctor\n";
	}
};

struct MemberFunc {
	int x;
	int getX() const {return x;}
	void setX(int xx) { x = xx;}
};

void anotherFunc(CopyCount const&) {}

int main(int argc, char *argv[]) {
	auto args_tuple = std::make_tuple(1,2,3.0);
	auto func2 = func;

	folly::applyTuple(func2, args_tuple);
	folly::applyTuple(func, args_tuple);
	folly::applyTuple(func, std::make_tuple(1,2,3.0));
	folly::applyTuple(makeFunc(), std::make_tuple(1,2,3.0));
	folly::applyTuple(makeFunc(), args_tuple);

	std::unique_ptr<Wat> wat(new Wat);
	// 此处调用 wat.get()，实际上是 unique_ptr 的一个成员函数
	folly::applyTuple(&Wat::func, std::make_tuple(wat.get(), 1,2,3.0));
	auto args_tuple2 = std::make_tuple(wat.get(), 1,2,3.0);
	folly::applyTuple(&Wat::func, args_tuple2);

	EXPECT_EQ(10.0,
			folly::applyTuple(&Wat::retVal, std::make_tuple(wat.get(), 1, 9.0)));

	auto test = guard(func, 1,2,3.0);
	CopyCount cpy;
	auto test2 = guard(anotherFunc, cpy);
	auto test3 = guard(anotherFunc, std::cref(cpy));

	Overloaded ovl;
	EXPECT_EQ(0,
			folly::applyTuple(
				// 此处 Overloaded 中有两个 func 函数，static_cast
				// 选择其中带 int 参数的那个
				static_cast<int (Overloaded::*)(int)>(&Overloaded::func),
				std::make_tuple(&ovl, 12)));

	EXPECT_EQ(true,
			folly::applyTuple(
				// 选择其中带 bool 参数的那个
				static_cast<bool (Overloaded::*)(bool)>(&Overloaded::func),
				std::make_tuple(&ovl, false)));

	int x = folly::applyTuple(std::plus<int>(), std::make_tuple(12, 12));
	EXPECT_EQ(24, x);
	
	Mover m;
	folly::applyTuple(move_only_func,
			std::forward_as_tuple(std::forward<Mover>(Mover())));
	const auto tuple3 = std::make_tuple(1,2,3.0);
	folly::applyTuple(func, tuple3);

	folly::applyTuple(
			// 此处，将 args_tuple 里面的元素提取出来，传递给 lambda
			// 函数，lambda 函数捕捉三个元素 a,b,c，并将其传递给 func()
			// 此处 mutable 不是必须的
			[](int a, int b, double c) mutable {func(a,b,c);}, args_tuple
			);

	ConstOverloaded covl;

	// 这里玄乎了：applyTuple 会将第一个参数当做函数来调用，此处
	// covl 实际上可以理解为函数调用 covl()，而这正是调用了 ConstOverloaded
	// 的函数重载符函数，有点精妙~

	// 调用 operator()()
	EXPECT_EQ(folly::applyTuple(covl, std::make_tuple()), 101);
	EXPECT_EQ(folly::applyTuple(std::ref(covl), std::make_tuple()), 101);
	EXPECT_EQ(folly::applyTuple(std::move(covl), std::make_tuple()), 101);
	// 调用 operator()() const 
	EXPECT_EQ(folly::applyTuple(const_cast<ConstOverloaded const&>(covl),
				std::make_tuple()), 102);
	// std::cref 返回 const reference, 无需 static_cast 了
	EXPECT_EQ(folly::applyTuple(std::cref(covl), std::make_tuple()), 102);

	MemberFunc mf;
	mf.x = 123;
	// 此处，传入 mf，并调用其成员函数 getX
	EXPECT_EQ(folly::applyTuple(&MemberFunc::getX, std::make_tuple(&mf)), 123);
	folly::applyTuple(&MemberFunc::setX, std::make_tuple(&mf, 234));

}
