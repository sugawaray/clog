#include "clog.h"
#include "nomagic.h"

#define L	(loc(__FILE__, __LINE__))

namespace {

using nomagic::loc;
using nomagic::Test;

using clog::Content;

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

void t2(const char* ms)
{
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	clog::out(m, f2, 1);
	t.a(Spy::last()->message == m, L);
	t.a(nf::called, L);
	t.a(nf::arg.v == 1, L);
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

void t3(const char* ms)
{
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	clog::out(m, f3, nf::a);
	t.a(Spy::last()->message == m, L);
	t.a(nf::called, L);
	t.a(nf::arg.v == nf::a.v, L);
	t.a(nf::A::copied == false, L);
}

void t4(const char* ms)
{
	Test t(ms);
	F f;
	const char* m("message");
	nf::reset();
	clog::out(m, f3c, nf::a);
	t.a(Spy::last()->message == m, L);
	t.a(nf::called, L);
	t.a(nf::arg.v == nf::a.v, L);
	t.a(nf::A::copied == false, L);
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
}

} // unnamed

int main()
{
	spy_tests();
	log_tests();
	return 0;
}
