#include "config_list_fixture.h"
#include "spy_fixture.h"
#include "test_ext.h"
#include <clog/clog.h>
#include <clog/time.h>
#include <nomagic.h>
#include <ctime>
#include <iostream>
#include <string>

#include "macros.h"

namespace test {

using nomagic::run;

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

long long fsuccess_ll()
{
	return 9223372036854775807;//at least LLONG_MAX
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
		get_configs()[0].store_return_value = false;
		clogcmn::Elapsed_time::clock_gettimefn = incrone;
	}

	void require_measure_etime(bool value) {
		get_configs()[0].measure_etime = value;
	}

	void require_store_return_value(bool value) {
		get_configs()[0].store_return_value = value;
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

namespace nme {

void t2(const char* ms)
{
	case_do_not_require_measuring_time(ms, fsuccess_v);
}

void t3(const char* ms)
{
	case_do_not_require_measuring_time(ms, fsuccess_i);
}

} // nme

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

namespace nme {

void t4(const char* ms)
{
	case_require_measuring_time(ms, fsuccess_v);
}

void t5(const char* ms)
{
	case_require_measuring_time(ms, fsuccess_i);
}

} // nme

template<class F>
void case_do_not_require_measuring_time_exception(const char* ms, F fn)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(false);
	try {
		(out(0, fn))();
	}
	catch (const E&) {
	}
	t.a(!Spy::last()->has_elapsed_time(), L);
}

namespace nme {

void t6(const char* ms)
{
	case_do_not_require_measuring_time_exception(ms, ferror_v);
}

} // nme

template<class F>
void case_require_measuring_time_exception(const char* ms, F fn)
{
	Test t(ms);
	Fixture f;
	f.require_measure_etime(true);
	try {
		(out(0, fn))();
	}
	catch (const E&) {
	}
	t.a(Spy::last()->has_elapsed_time(), L);
	t.a(Spy::last()->elapsed_time == 1000001, L);
}

namespace nme {

void t7(const char* ms)
{
	case_require_measuring_time_exception(ms, ferror_v);
}

void t8(const char* ms)
{
	case_do_not_require_measuring_time_exception(ms, ferror_i);
}

void t9(const char* ms)
{
	case_require_measuring_time_exception(ms, ferror_i);
}

void run_measure_etime_tests()
{
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

	run(	"It does not measure elapsed time if its flag is not set "
		"when the function throws exception.(return type not void",
		t8);

	run(	"It measures elapsed time if its flag is set "
		"when the function throws exception.(return type not void",
		t9);
}

} // nme

namespace nrv {

using std::string;

// ex means an expected value.
template<class F>
void case_does_not_store_value(const char* ms, F f, const string& ex)
{
	Test t(ms);
	Fixture fi;
	(out(0, f))();
	t.a(Spy::last()->return_value.to_string() == ex, L);
}

// ex means an expected value.
template<class F, class Fv>
void case_stores_value(const char* ms, F f, Fv fv, const string& ex)
{
	Test t(ms);
	Fixture fi;
	fi.require_store_return_value(true);
	(out(0, f))();
	fv(t, Spy::last()->return_value.to_string(), ex, L);
}

void t1(const char* ms)
{
	case_does_not_store_value(ms, fsuccess_i, clogcmn::Notset);
}

void t2(const char* ms)
{
	case_does_not_store_value(ms, fsuccess_v, clogcmn::Notset);
}

void verify_simple(Test& t, const string& op1, const string& op2,
	const string& m)
{
	t.a(op1 == op2, m);
}

void t3(const char* ms)
{
	case_stores_value(ms, fsuccess_i, verify_simple, "1");
}

void t4(const char* ms)
{
	case_stores_value(ms, fsuccess_ll, verify_simple,
		"9223372036854775807");
}

void run_return_value_tests()
{
	std::cerr << "extended log tests:return value" << std::endl;

	run(	"It does not store a return value when it is not asked to "
		"do it.", t1);

	run(	"It does not store a return value when the return type is "
		"void.", t2);

	run("type int", t3);

	run("type long long int", t4);
}

} // nrv

} // unnamed

void run_clog_extension_tests()
{
	run("It does not cause errors when config_list is 0.", t1);

	nme::run_measure_etime_tests();

	nrv::run_return_value_tests();
}

} // test
