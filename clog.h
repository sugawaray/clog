#ifndef __NCLOG_CLOG_H__
#define __NCLOG_CLOG_H__

#include <string>

namespace {
namespace nclog {

using std::string;

struct Content {
	string message;

	Content(const string& message)
		:	message(message) {
	}
};

void outimpl(const Content&)
{
}

typedef void (*Out)(const Content&);
Out out(outimpl);

void outlog(const char* m, void (*f)())
{
	f();
	out(Content(m));
}

} // nclog
} // unnamed

#endif // __NCLOG_CLOG_H__
