#include "test.h"
#include "test_ext.h"
#include "spy_test.h"
#include "time_test.h"
#include "var_test.h"
#include <clog/clog.h>
#include <an_impl/outimpl.h>
#include <an_impl/outimpl_test.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;

namespace {

// retval means return value.
#define DEFLOG(id, m, etime, retval)	CHOOSE(id, LOGDEF(m, etime, retval))
#define LOGDEF(m, etime, retval)	{ m, etime, retval }
#define LOGDEFLIST	\
DEFLOG(LEVENT1, "some event 1 happened", true, true),	\
DEFLOG(LEVENT2, "some event 2 happened", true, true),	\
DEFLOG(LEVENT3, "some event 3 happened", false, true)	\
/**/
#define CHOOSE(id, def) id
enum {
	LOGDEFLIST
};
#undef CHOOSE
#define CHOOSE(id, def)	def
clogcmn::Config logconfigs[] = {
	LOGDEFLIST
};

clogcmn::Config_list logconfig_list(clogcmn::Config_list::create(logconfigs));

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

int return_rand()
{
	return std::rand() % 1000;
}

int fi0wait1()
{
	usleep(1000);
	return return_rand();
}

int fi0wait2()
{
	usleep(2000);
	return return_rand();
}

int fi1wait(int v)
{
	usleep(v);
	return return_rand();
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
	std::srand(std::time(0));
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

	clog::config_list = &logconfig_list;
	clogcmn::Elapsed_time::clock_gettimefn = clock_gettime;
	L0(LEVENT1, fi0wait1);
	L0(LEVENT2, fi0wait2);
	L1(LEVENT1, fi1wait, 300);

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
	clogcmn::test::run_var_tests();
	impl::test::run_outimpl_tests();
	cout << "test end" << endl;

	example();

	return 0;
}
