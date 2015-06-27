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
	A() : v(0) {
	}

	A(const A& op)
		:	v(op.v) {
		copied = true;
	}

	int v;
	static bool copied;
};
bool A::copied;

A a;
A arg;

void reset()
{
	called = false;
	a = A();
	a.v = 1;
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

void t1(const char* ms)
{
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	clog::out(m, f1);
	t.a(Spy::last()->message == m, L);
	t.a(nf::called, L);
}

void f2(int a)
{
	nf::called = true;
	nf::arg.v = a;
}

class Test_1 {
public:
	Test_1(const char* ms)
		:	t(ms) {
	}

	void operator()() {
		F f;
		m = "message";
		nf::reset();
		prepare_call();
		call_and_assert();
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
		t.a(nf::arg.v == nf::a.v, L);
		t.a(nf::A::copied == false, L);
	}
protected:
	virtual void call_and_assert() = 0;
	virtual void prepare_call() {
	}

	Test t;
	const char* m;
};

namespace d {

struct Ret_not_void { };
struct Ret_void { };

template<class T>
struct Ret_tag {
	typedef Ret_not_void type;
};
template<>
struct Ret_tag<void> {
	typedef Ret_void type;
};

} // d

template<class F>
class Test_1v : public Test_1 {
public:
	Test_1v(F f, const char* ms) : Test_1(ms), f(f) {
	}

protected:
	void call_and_assert() {
		typedef typename d::Ret_tag<
			decltype(clog::out(m, f, nf::a.v))
				>::type tag;
		call(tag());
	}
	void prepare_call() {
		nf::a.v = 2;
	}
private:
	template<class T>
	void call(T forwarder) {
		t.a(clog::out(m, f, nf::a.v) == 1, L);
	}

	void call(d::Ret_void dummy) {
		clog::out(m, f, nf::a.v);
	}

	F f;
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
class Test_1r : public Test_1 {
public:
	Test_1r(F f, const char* ms) : Test_1(ms), f(f) {
	}

protected:
	void call_and_assert() {
		typedef typename d::Ret_tag<
			decltype(clog::out(m, f, nf::a))
				>::type tag;
		call(tag());
	}
private:
	template<class T>
	void call(T forwarder) {
		t.a(clog::out(m, f, nf::a) == 1, L);
	}

	void call(d::Ret_void dummy) {
		clog::out(m, f, nf::a);
	}

	F f;
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

void t8(const char* ms)
{
	(test_1r(f1_1r, ms))();
}

void t9(const char* ms)
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
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	int r(clog::out(m, f1_0));
	t.a(r == 1, L);
	t.a(Spy::last()->message == m, L);
	t.a(nf::called, L);
}

int f1_1(int v)
{
	nf::called = true;
	nf::arg.v = v;
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

void t7(const char* ms)
{
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	try {
		clog::out(m, f0e);
		throw std::logic_error("test");
	}
	catch (const nf::E&) {
	}
	t.a(Spy::last()->message == m, L);
	t.a(Spy::last()->exception, L);
}

} // nlog

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
	run("return(int), arg(1, ref)", t8);
	run("return(int), arg(1, cref)", t9);
	run("return(void), arg(0), exception", t7);
}

} // unnamed

#include "test.h"

namespace test {

void run_clog_tests()
{
	spy_tests();
	log_tests();
}

} // test

