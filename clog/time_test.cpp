#include "time.h"
#include "time_test.h"
#include <nomagic.h>
#include <cstddef>
#include <ctime>
#include <iostream>

#include <macros.h>

namespace clogcmn {
namespace test {
namespace {

using std::size_t;
using nomagic::Test;

clock_t clock_now;

clock_t return_fixed_value()
{
	return clock_now;
}

class Fixture : public Test {
public:
	Fixture(const char* ms)
		:	Test(ms) {
		Elapsed_time::clockfn = return_fixed_value;
	}
};

void t1(const char* ms)
{
	Fixture t(ms);
	size_t r;
	t.a(Elapsed_time().now(&r) == false, L);
}

void t2(const char* ms)
{
	Fixture t(ms);
	Elapsed_time time;
	time.start();
	t.a(time.now(0) == false, L);
}

void t3(const char* ms)
{
	Fixture t(ms);
	Elapsed_time et;
	clock_now = 1;
	et.start();
	clock_now = 3;
	size_t r;
	t.a(et.now(&r), L);
	t.a(r == 2, L);
}

void t4(const char* ms)
{
	Fixture t(ms);
	Elapsed_time et;
	clock_now = 1;
	et.start();
	clock_now = 2;
	et.start();
	clock_now = 3;
	size_t r;
	t.a(et.now(&r) == 1, L);
}

} // unnamed

void run_elapsed_time_tests()
{
	std::cerr << "Elapsed_time tests" << std::endl;

	using nomagic::run;
	run(	"now returns false when start() "
		"has not been called before.", t1);

	run("now returns false when a result address is not specified.", t2);

	run("now returns clock counts.", t3);

	run("start() sets the start point of a duration.", t4);
}

} // test
} // clogcmn
