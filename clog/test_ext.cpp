#include "spy_fixture.h"
#include "test_ext.h"
#include "clog.h"
#include "time.h"
#include <nomagic.h>
#include <ctime>
#include <iostream>

#include <macros.h>

namespace test {
namespace {

struct E {
};

void fsuccess_v()
{
}

int fsuccess_i()
{
	return 1;
}

void ferror_v()
{
	throw E();
}

int ferror_i()
{
	throw E();
}

using clog::config_list;
using clog::out;
using clogcmn::Config;
using clogcmn::Config_list;
using nomagic::Test;

void t1(const char* ms)
{
	Test t(ms);
	Spy_fixture f;
	config_list = 0;
	(out(0, fsuccess_v))();
	(out(0, fsuccess_i))();
	try {
		(out(0, ferror_v))();
	}
	catch (const E&) {
	}
	try {
		(out(0, ferror_i))();
	}
	catch (const E&) {
	}
}

template<int N>
class Config_list_fixture {
public:
	Config_list_fixture() {
		list = Config_list::create(configs);
		config_list = &list;
	}

	Config (&get_configs())[N] {
		return configs;
	}
private:
	Config configs[N];
	Config_list list;
};

int incrone(clockid_t c, timespec* b)
{
	static std::time_t s(1);
	static long n(0);
	b->tv_sec = s;
	b->tv_nsec = n;
	++s;
	++n;
	return 0;
}

class Fixture : private Spy_fixture, private Config_list_fixture<1> {
public:
	Fixture() {
		get_configs()[0].message = "message";
		get_configs()[0].measure_etime = false;
		clogcmn::Elapsed_time::clock_gettimefn = incrone;
	}

	void require_measure_etime(bool value) {
		get_configs()[0].measure_etime = value;
	}
};

template<class F>
void case_do_not_require_measuring_time(const char* ms, F fn)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(false);
	(out(0, fn))();
	t.a(!Spy::last()->has_elapsed_time(), L);
}

void t2(const char* ms)
{
	case_do_not_require_measuring_time(ms, fsuccess_v);
}

void t3(const char* ms)
{
	case_do_not_require_measuring_time(ms, fsuccess_i);
}

template<class F>
void case_require_measuring_time(const char* ms, F fn)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(true);
	(out(0, fn))();
	t.a(Spy::last()->has_elapsed_time(), L);
	t.a(Spy::last()->elapsed_time == 1000001, L);
}

void t4(const char* ms)
{
	case_require_measuring_time(ms, fsuccess_v);
}

void t5(const char* ms)
{
	case_require_measuring_time(ms, fsuccess_i);
}

void t6(const char* ms)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(false);
	try {
		(out(0, ferror_v))();
	}
	catch (const E&) {
	}
	t.a(!Spy::last()->has_elapsed_time(), L);
}

void t7(const char* ms)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(true);
	try {
		(out(0, ferror_v))();
	}
	catch (const E&) {
	}
	t.a(Spy::last()->has_elapsed_time(), L);
	t.a(Spy::last()->elapsed_time == 1000001, L);
}

} // unnamed

void run_clog_extension_tests()
{
	using nomagic::run;

	run("It does not cause errors when config_list is 0.", t1);

	run(	"It does not measure elapsed time if its flag is not set "
		"when return type is void.", t2);

	run(	"It does not measure elapsed time if its flag is not set "
		"when return type is not void.", t3);

	run(	"It measures elapsed time if its flag is set "
		"when return type is void.", t4);

	run(	"It measures elapsed time if its flag is set "
		"when return type is not void.", t5);

	run(	"It does not measure elapsed time if its flag is not set "
		"when the function throws exception.", t6);

	run(	"It measures elapsed time if its flag is set "
		"when the function throws exception.", t7);
}

} // test
