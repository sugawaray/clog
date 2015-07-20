#include <clog/content.h>
#include "var_test.h"
#include <nomagic.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "macros.h"

namespace clogcmn {
namespace test {
namespace {

template<class T>
Var var(T v)
{
	Var o;
	o.set(v);
	return o;
}

void t1(const char* ms)
{
	nomagic::Test t(ms);
	t.a(var<char>(127).to_string() == "127", L);
	t.a(var<char>(-127).to_string() == "-127", L);
	t.a(var<unsigned char>(255).to_string() == "255", L);
	t.a(var<short>(32767).to_string() == "32767", L);
	t.a(var<short>(-32767).to_string() == "-32767", L);
	t.a(var<unsigned short>(65535).to_string() == "65535", L);
	t.a(var<int>(32767).to_string() == "32767", L);
	t.a(var<int>(-32767).to_string() == "-32767", L);
	t.a(var<unsigned int>(65535).to_string() == "65535", L);
	t.a(var<long>(2147483647L).to_string() == "2147483647", L);
	t.a(var<long>(-2147483647L).to_string() == "-2147483647", L);
	t.a(var<unsigned long>(4294967295UL).to_string() == "4294967295", L);
	t.a(var<long long>(9223372036854775807LL).to_string() ==
		"9223372036854775807", L);
	t.a(var<long long>(-9223372036854775807LL).to_string() ==
		"-9223372036854775807", L);
	t.a(var<unsigned long long>(18446744073709551615ULL).to_string() ==
		"18446744073709551615", L);
	std::stringstream ss;
	ss.str(var<float>(9.9999995).to_string());
	float fv;
	ss >> fv;
	t.a(std::fabs(fv - 9.9999995) < 0.000001, L);

	ss.str(var<double>(9.9999995).to_string());
	ss.clear();
	double dv;
	ss >> dv;
	t.a(std::abs(dv - 9.9999995) < 0.000001, L);

	ss.str(var<long double>(9.9999995).to_string());
	ss.clear();
	long double ld;
	ss >> ld;
	t.a(std::fabs(ld - 9.9999995) < 0.000001, L);
}

} // unnamed

void run_var_tests()
{
	std::cerr << "clogcmn::Var tests" << std::endl;
	nomagic::run("testall", t1);
}

} // test
} // clogcmn
