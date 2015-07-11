#ifndef __TEST_SPY_H__
#define __TEST_SPY_H__

#include <clog/content.h>

namespace test {

using clogcmn::Content;

class Spy {
public:
	static Content* last() {
		return p;
	}

	static void out(const Content& content) {
		if (q == 0)
			q = new Content(content.message);
		p = q;
		*p = content;
	}

	static void reset() {
		p = 0;
	}

private:
	static Content* p;
	static Content* q;
};

} // test

#endif // __TEST_SPY_H__
