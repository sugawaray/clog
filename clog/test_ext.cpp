#include "test_ext.h"
#include "clog.h"
#include <nomagic.h>
#include <iostream>

namespace test {
namespace {

void t1(const char* ms)
{
}

} // unnamed

void run_clog_extension_tests()
{
	using nomagic::run;

	run("It does not cause errors when config_list is 0.", t1);
}

} // test
