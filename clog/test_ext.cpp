#include "spy_fixture.h"
#include "test_ext.h"
#include "clog.h"
#include <nomagic.h>
#include <iostream>

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

void t1(const char* ms)
{
	nomagic::Test t(ms);
	Spy_fixture f;
	clog::config_list = 0;
	(clog::out(0, fsuccess_v))();
	(clog::out(0, fsuccess_i))();
	try {
		(clog::out(0, ferror_v))();
	}
	catch (const E&) {
	}
	try {
		(clog::out(0, ferror_i))();
	}
	catch (const E&) {
	}
}

} // unnamed

void run_clog_extension_tests()
{
	using nomagic::run;

	run("It does not cause errors when config_list is 0.", t1);
}

} // test
