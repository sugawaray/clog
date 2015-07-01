#include <clog/clog.h>
#include <clog/test.h>
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

void example()
{
	cout << "some examples start" << endl;

	impl::stream = &cout;
	clog::outfn = impl::outimpl;
	namespace n = clog;
	if ((n::out("fb0", fb0))()) {
		if ((n::out("fi1", fi1))(0) == 0) {
			if ((n::out("fi1", fi1))(1) == 1) {
				;
			}
		}
	}
	try {
		(n::out("fe", fe))();
	}
	catch (const runtime_error& e) {
		cout << "but it's expected." << endl;
	}

	cout << "examples end" << endl;
}

} // unnamed

int main()
{
	cout << "test start" << endl;
	test::run_clog_tests();
	impl::test::run_outimpl_tests();
	cout << "test end" << endl;

	example();

	return 0;
}
