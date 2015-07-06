#ifndef __SPY_FIXTURE_H__
#define __SPY_FIXTURE_H__

#include "clog.h"
#include "spy.h"
#include <iostream>

namespace {

class Spy_fixture {
public:
	// This function must be inline.
	Spy_fixture() {
		using test::Spy;
		clog::outfn = Spy::out;
		Spy::reset();
	}
};

} // unnamed

#endif // __SPY_FIXTURE_H__
