#ifndef __CLOGCMN_CONTENT_H__
#define __CLOGCMN_CONTENT_H__

#include <cstddef>
#include <string>

namespace clogcmn {

using std::string;

struct Content {
	string message;
	bool exception;
	long elapsed_time;

	bool elapsed_time_valid;

	bool has_elapsed_time() const {
		return elapsed_time_valid;
	}

	Content()
		:	exception(false),
			elapsed_time(0),
			elapsed_time_valid(false) {
	}

	Content(const string& message)
		:	message(message),
			exception(false),
			elapsed_time(0),
			elapsed_time_valid(false) {
	}
};

} // clogcmn

#endif // __CLOGCMN_CONTENT_H__
