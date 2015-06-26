#include "outimpl_test.h"
#include "test.h"

int main()
{
	test::run_clog_tests();
	clog::test::run_outimpl_tests();
	return 0;
}
