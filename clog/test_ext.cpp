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

void t2(const char* ms)
{
	Test t(ms);
	Spy_fixture f;
	Config_list_fixture<1> cf;
	cf.get_configs()[0].message = "message";
	cf.get_configs()[0].measure_etime = false;
	(out(0, fsuccess_v))();
	t.a(!Spy::last()->has_elapsed_time(), L);
}

std::clock_t zero_one()
{
	static int r(0);
	return r++ % 2;
}

void t3(const char* ms)
{
	Test t(ms);
	Spy_fixture f;
	Config_list_fixture<1> cf;
	cf.get_configs()[0].message = "message";
	cf.get_configs()[0].measure_etime = true;
	(out(0, fsuccess_v))();
	clogcmn::Elapsed_time::clockfn = zero_one;
	t.a(Spy::last()->has_elapsed_time(), L);
	t.a(Spy::last()->elapsed_clocks == 1, L);
}

} // unnamed

void run_clog_extension_tests()
{
	using nomagic::run;

	run("It does not cause errors when config_list is 0.", t1);

	run("It does not measure elapsed time if its flag is not set.", t2);

	run("It measures elapsed time if its flag is set.", t3);
}

} // test
