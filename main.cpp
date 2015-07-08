#include <clog/clog.h>
#include <clog/test.h>
#include <clog/test_ext.h>
#include <clog/spy_test.h>
#include <clog/time_test.h>
#include "an_impl/outimpl.h"
#include "an_impl/outimpl_test.h"
#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;

namespace {

bool fb0()
{
	return true;
}

int fi1(int v)
{
	return v;
}

using std::runtime_error;

void fe()
{
	throw runtime_error("exception");
}

class Sample {
public:
	void mv0() {
	}
};

#define L0(m, f)	((n::out((m), (f)))())
#define L1(m, f, a1)	((n::out((m), (f)))((a1)))
#define LM0(m, p, o)	((n::out((m), (p)))((o)))
void example()
{
	cout << "some examples start" << endl;

	impl::stream = &cout;
	clog::outfn = impl::outimpl;
	namespace n = clog;
	if (L0("calling fb0 ...", fb0)) {
		if (L1("calling fi1 ...", fi1, 0) == 0) {
			if (L1("calling fi1 ...", fi1, 1) == 1) {
				;
			}
		}
	}
	try {
		L0("calling fe which may throw an exception.", fe);
	}
	catch (const runtime_error& e) {
		cout << "but it's expected." << endl;
	}
	Sample sample;
	LM0("calling a method of the Sample object.", &Sample::mv0, sample);

	cout << "examples end" << endl;
}

} // unnamed

int main()
{
	cout << "test start" << endl;
	test::run_clog_spy_tests();
	test::run_clog_tests();
	test::run_clog_extension_tests();
	clogcmn::test::run_elapsed_time_tests();
	impl::test::run_outimpl_tests();
	cout << "test end" << endl;

	example();

	return 0;
}
