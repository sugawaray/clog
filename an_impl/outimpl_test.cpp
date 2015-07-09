#include "outimpl.h"
#include "outimpl_test.h"
#include <clog/content.h>
#include <utils.h>
#include <nomagic.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <macros.h>

namespace impl {
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

void t4(const char* ms)
{
	using std::string;
	Test t(ms);
	Content c;
	c.message = "message";
	c.elapsed_time = 1000000000;
	c.elapsed_time_valid = true;

	string r(to_string(c));
	t.a(r.find("message(duration:") == 0, L);
	r = r.substr(string::traits_type::length("message(duration:"));
	double d(std::strtod(r.c_str(), 0));
	t.a(fabs(d - 1000) < 0.0000001, L);
	t.a(r.substr(r.find_first_not_of("0123456789.")) == ")", L);
}

} // unnamed

void run_outimpl_tests()
{
	using nomagic::run;

	run("It outputs a message.", t1);

	run("to_string : It outputs correctly.", t2);

	run(	"to_string : It outputs correctly "
		"when an exception is thrown.", t3);

	run("It outputs duration if there is a duration.", t4);
}

} // test
} // impl
