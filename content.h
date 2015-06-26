#ifndef __CLOG_CONTENT_H__
#define __CLOG_CONTENT_H__

#include <string>

namespace clog {

using std::string;

struct Content {
	string message;
	bool exception;

	Content(const string& message)
		:	message(message),
			exception(false) {
	}
};

} // clog

#endif // __CLOG_CONTENT_H__
