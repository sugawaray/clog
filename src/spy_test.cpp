#include "spy_test.h"
#include "spy.h"
#include <nomagic.h>

#include "macros.h"

namespace {

using nomagic::Test;
using test::Spy;
using clogcmn::Content;

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

} // unnamed

namespace test {

void run_clog_spy_tests()
{
	using nomagic::run;

	run(	"Spy returns null when it is asked to "
		"provide the last log output but there is not.", t1);

	run("Spy returns the last log object", t2);

	run("Spy resets the last log", t3);
}

} // test
