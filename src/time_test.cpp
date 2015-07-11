#include "time_test.h"
#include <clog/time.h>
#include <nomagic.h>
#include <cstddef>
#include <ctime>
#include <iostream>

#include "macros.h"

namespace clogcmn {
namespace test {
namespace {

using nomagic::Test;

timespec time_now;

int return_fixed_time(clockid_t id, timespec* p)
{
	*p = time_now;
	return 0;
}

class Fixture : public Test {
public:
	Fixture(const char* ms)
		:	Test(ms) {
		Elapsed_time::clock_gettimefn = return_fixed_time;
	}
};

void t1(const char* ms)
{
	Fixture t(ms);
	long r;
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
	time_now.tv_sec = 1;
	time_now.tv_nsec = 2;
	et.start();
	time_now.tv_sec = 3;
	time_now.tv_nsec = 4;
	long r;
	t.a(et.now(&r), L);
	t.a(r == 2000002, L);
}

void t4(const char* ms)
{
	Fixture t(ms);
	Elapsed_time et;
	time_now.tv_sec = 1;
	time_now.tv_nsec = 2;
	et.start();
	time_now.tv_sec = 3;
	time_now.tv_nsec = 4;
	et.start();
	time_now.tv_sec = 5;
	time_now.tv_nsec = 6;
	long r;
	t.a(et.now(&r), L);
	t.a(r == 2000002, L);
}

} // unnamed

void run_elapsed_time_tests()
{
	std::cerr << "Elapsed_time tests" << std::endl;

	using nomagic::run;
	run(	"now returns false when start() "
		"has not been called before.", t1);

	run("now returns false when a result address is not specified.", t2);

	run("now returns elapsed duration in nano seconds.", t3);

	run("start() sets the start point of a duration.", t4);
}

} // test
} // clogcmn
