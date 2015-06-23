#include "clog.h"
#include "nomagic.h"

#define L	(loc(__FILE__, __LINE__))

namespace nspy {

using nclog::Content;

class Spy {
public:
	static Content* last() {
		return p;
	}

	static void out(const Content& content) {
		if (p == 0)
			p = new Content(content.message);
		else
			*p = content;
	}

private:
	static Content* p;
};

Content* Spy::p(0);

using nomagic::loc;
using nomagic::Test;

void t1(const char* ms)
{
	Test t(ms);
	t.a(Spy::last() == 0, L);
}

void t2(const char* ms)
{
	Test t(ms);
	const char* m("a message");
	Content c(m);
	Spy::out(c);
	Content* r(Spy::last());
	t.a(r != 0, L);
	t.a(r->message == m, L);
}

} // nspy

void spy_tests()
{
	using nomagic::run;
	using namespace nspy;

	run(	"Spy returns null when it is asked to "
		"provide the last log output but there is not.", t1);

	run("Spy returns the last log object", t2);
}

int main()
{
	spy_tests();
	return 0;
}
