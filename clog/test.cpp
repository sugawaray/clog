#include <clog/clog.h>
#include <nomagic.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "macros.h"

namespace {

using nomagic::loc;
using nomagic::Test;

using clogcmn::Content;

class Spy {
public:
	static Content* last() {
		return p;
	}

	static void out(const Content& content) {
		if (q == 0)
			q = new Content(content.message);
		p = q;
		*p = content;
	}

	static void reset() {
		p = 0;
	}

private:
	static Content* p;
	static Content* q;
};

Content* Spy::p(0);
Content* Spy::q(0);

namespace nspy {

void t1(const char* ms)
{
	Test t(ms);
	t.a(Spy::last() == 0, L);
}

void test_out(Test& t, const char* m)
{
	Content c(m);
	Spy::out(c);
	Content* r(Spy::last());
	t.a(r != 0, L);
	t.a(r->message == m, L);
}

void t2(const char* ms)
{
	Test t(ms);
	test_out(t, "a message 1");
	Spy::reset();
	test_out(t, "a message 2");
}

void t3(const char* ms)
{
	Test t(ms);
	Content c("a message");
	Spy::out(c);
	Spy::reset();
	Content* r(Spy::last());
	t.a(r == 0, L);
}

} // nspy

namespace nlog {

class F {
public:
	F() {
		clog::outfn = Spy::out;
	}
};

namespace nf {

bool called;

class A {
public:
	A() : v1(0), v2(0) {
	}

	A(const A& op)
		:	v1(op.v1), v2(op.v2) {
		copied = true;
	}

	int v1;
	int v2;
	static bool copied;
};
bool A::copied;

A a;
A arg;

const int default_v1(1);
const int default_v2(2);

void reset()
{
	called = false;
	a = A();
	a.v1 = 1;
	a.v2 = 2;
	arg = A();
	A::copied = false;
}

struct E {
};

} // nf

void f1()
{
	nf::called = true;
}

namespace d {

using std::enable_if;
using std::is_void;

template<class T>
struct enable_void : enable_if<is_void<T>::value> {
};

template<class T>
struct disable_void : enable_if<!is_void<T>::value> {
};

} // d

class Test_common {
public:
	Test_common(const char* ms)
		:	t(ms) {
	}

	void operator()() {
		F f;
		m = "message";
		nf::reset();
		prepare_call();
		call_and_assert();
		verify();
	}
protected:
	virtual void call_and_assert() = 0;
	virtual void prepare_call() {
	}
	virtual void verify() = 0;

	Test t;
	const char* m;
};

template<class F>
class Test_0 : public Test_common {
public:
	Test_0(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}
protected:
	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
	}

	F f;
private:
	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))();
	}
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))() == 1, L);
	}
};

using std::logic_error;

template<class F>
class Test_ex : public Test_common {
public:
	Test_ex(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}

protected:
	void call_and_assert() {
		try {
			call();
			logic_error("test");
		}
		catch (const nf::E&) {
		}
	}

	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(Spy::last()->exception, L);
	}

	virtual void call() = 0;

	F f;
};

template<class F>
class Test_0_ex : public Test_ex<F> {
public:
	Test_0_ex(F f, const char* ms)
		:	Test_ex<F>(f, ms) {
	}

protected:
	using Test_ex<F>::f;
	using Test_ex<F>::m;

	void call() {
		(clog::out(m, f))();
	}
};

template<class F>
class Test_1_ex : public Test_ex<F> {
public:
	Test_1_ex(F f, const char* ms)
		:	Test_ex<F>(f, ms) {
	}

protected:
	using Test_ex<F>::f;
	using Test_ex<F>::m;

	void call() {
		(clog::out(m, f))(1);
	}
};

template<class F>
inline Test_0_ex<F> test_0_ex(F f, const char* ms)
{
	return Test_0_ex<F>(f, ms);
}

template<class F>
inline Test_1_ex<F> test_1_ex(F f, const char* ms)
{
	return Test_1_ex<F>(f, ms);
}

template<class F>
inline Test_0<F> test_0(F f, const char* ms)
{
	return Test_0<F>(f, ms);
}

void t1(const char* ms)
{
	(test_0(f1, ms))();
}

void f2(int a)
{
	nf::called = true;
	nf::arg.v1 = a;
}

template<class F>
class Test_1 : public Test_common {
public:
	Test_1(F f, const char* ms) : Test_common(ms), f(f) {
	}

protected:
	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
		t.a(nf::arg.v1 == nf::a.v1, L);
		t.a(nf::A::copied == false, L);
	}

	F f;
};

template<class F>
class Test_2 : public Test_common {
public:
	Test_2(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}

protected:
	void prepare_call() {
		nf::a.v1 = nf::default_v1 + 1;
		nf::a.v2 = nf::default_v2 + 1;
	}

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}

	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
		t.a(nf::arg.v1 == nf::a.v1, L);
		t.a(nf::arg.v2 == nf::a.v2, L);
	}
private:
	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a.v1, nf::a.v2);
	}

	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a.v1, nf::a.v2) == 10, L);
	}

	F f;
};

template<class F>
class Test_1v : public Test_1<F> {
public:
	Test_1v(F f, const char* ms) : Test_1<F>(f, ms) {
	}

protected:
	using Test_1<F>::m;
	using Test_1<F>::t;
	using Test_1<F>::f;

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
	void prepare_call() {
		nf::a.v1 = 2;
		nf::a.v2 = nf::arg.v2;
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a.v1) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a.v1);
	}
};

template<class F>
inline Test_1v<F> test_1v(F f, const char* ms)
{
	return Test_1v<F>(f, ms);
}

void t2(const char* ms)
{
	(test_1v(f2, ms))();
}

void f3(nf::A& a)
{
	nf::called = true;
	nf::arg = a;
}

void f3c(const nf::A& a)
{
	nf::called = true;
	nf::arg = a;
}

template<class F>
class Test_1r : public Test_1<F> {
public:
	Test_1r(F f, const char* ms) : Test_1<F>(f, ms) {
	}

protected:
	using Test_1<F>::m;
	using Test_1<F>::t;
	using Test_1<F>::f;

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a);
	}
};

template<class F>
inline Test_1r<F> test_1r(F f, const char* ms)
{
	return Test_1r<F>(f, ms);
}

void t3(const char* ms)
{
	(test_1r(f3, ms))();
}

int f1_1r(nf::A& a)
{
	nf::called = true;
	nf::arg = a;
	return 1;
}

int f1_1rc(const nf::A& a)
{
	nf::called = true;
	nf::arg = a;
	return 1;
}

void t7(const char* ms)
{
	(test_1r(f1_1r, ms))();
}

void t8(const char* ms)
{
	(test_1r(f1_1rc, ms))();
}

void t4(const char* ms)
{
	(test_1r(f3c, ms))();
}

int f1_0()
{
	nf::called = true;
	return 1;
}

void t5(const char* ms)
{
	(test_0(f1_0, ms))();
}

int f1_1(int v)
{
	nf::called = true;
	nf::arg.v1 = v;
	return 1;
}

void t6(const char* ms)
{
	(test_1v(f1_1, ms))();
}

void f0e()
{
	throw nf::E();
}

void t9(const char* ms)
{
	(test_0_ex(f0e, ms))();
}

int f0ei()
{
	throw nf::E();
}

void t10(const char* ms)
{
	(test_0_ex(f0ei, ms))();
}

void f1e(int a)
{
	throw nf::E();
}

int f1ei(int a)
{
	throw nf::E();
}

void t11(const char* ms)
{
	(test_1_ex(f1e, ms))();
}

void t12(const char* ms)
{
	(test_1_ex(f1ei, ms))();
}

template<class F>
inline Test_2<F> test_2(F f, const char* ms)
{
	return Test_2<F>(f, ms);
}

void fv2(int v1, int v2)
{
	nf::called = true;
	nf::arg.v1 = v1;
	nf::arg.v2 = v2;
}

void t14(const char* ms)
{
	(test_2(fv2, ms))();
}

int fi2(int v1, int v2)
{
	nf::called = true;
	nf::arg.v1 = v1;
	nf::arg.v2 = v2;
	return 10;
}

void t15(const char* ms)
{
	(test_2(fi2, ms))();
}

} // nlog

namespace nlogm {

class Sample {
public:
	Sample()
		:	called(false) {
	}

	bool is_called() const {
		return called;
	}

	void mv0() {
		called = true;
	}
private:
	bool called;
};

void t1(const char* ms)
{
	Test t(ms);
	nlog::F f;
	Spy::reset();
	Sample sample;
	const char* m("message");
	(clog::out(m, &Sample::mv0))(sample);
	t.a(Spy::last()->message == m, L);
	t.a(sample.is_called(), L);
}

} // nlogm

using nomagic::run;

void spy_tests()
{
	using namespace nspy;

	run(	"Spy returns null when it is asked to "
		"provide the last log output but there is not.", t1);

	run("Spy returns the last log object", t2);

	run("Spy resets the last log", t3);
}

void log_tests()
{
	using namespace nlog;

	run("return(void), arg(0)", t1);
	run("return(void), arg(1)", t2);
	run("return(void), arg(1, ref)", t3);
	run("return(void), arg(1, cref)", t4);
	run("return(int), arg(0)", t5);
	run("return(int), arg(1)", t6);
	run("return(int), arg(1, ref)", t7);
	run("return(int), arg(1, cref)", t8);
	run("return(void), arg(0), exception", t9);
	run("return(int), arg(0), exception", t10);
	run("return(void), arg(1)", t11);
	run("return(int), arg(1)", t12);
	run("return(void), arg(2)", t14);
	run("return(int), arg(2)", t15);
}

void log_method_tests()
{
	using namespace nlogm;

	run("method, return(void), arg(0)", t1);
}

} // unnamed

#include "test.h"

namespace test {

void run_clog_tests()
{
	spy_tests();
	log_tests();
	log_method_tests();
}

} // test

