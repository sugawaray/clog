#include "content.h"
#include "outimpl.h"
#include "outimpl_test.h"
#include "utils.h"
#include <nomagic.h>
#include <iostream>
#include <sstream>

#include "macros.h"

namespace clog {
namespace test {

namespace {

void reset_stream()
{
	stream = &std::cout;
}

using std::ostringstream;
using nomagic::Test;
using utils::Auto_caller;

void t1(const char* ms)
{
	auto a(Auto_caller(reset_stream));
	Test t(ms);
	Content c("message");
	ostringstream os;
	stream = &os;
	outimpl(c);
	t.a(os.str() != "", L);
}

void t2(const char* ms)
{
	Test t(ms);
	Content c("message");
	t.a(to_string(c) == "message", L);
}

void t3(const char* ms)
{
	Test t(ms);
	Content c("message");
	c.exception = true;
	t.a(to_string(c) == "message(an exception is thrown)", L);
}

} // unnamed

void run_outimpl_tests()
{
	using nomagic::run;

	run("It outputs a message.", t1);

	run("to_string : It outputs correctly.", t2);

	run(	"to_string : It outputs correctly "
		"when an exception is thrown.", t3);
}

} // test
} // clog
