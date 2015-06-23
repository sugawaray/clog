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

} // nclog
} // unnamed

#endif // __NCLOG_CLOG_H__
