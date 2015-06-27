#ifndef __CLOGCMN_CONTENT_H__
#define __CLOGCMN_CONTENT_H__

#include <string>

namespace clogcmn {

using std::string;

struct Content {
	string message;
	bool exception;

	Content(const string& message)
		:	message(message),
			exception(false) {
	}
};

} // clogcmn

#endif // __CLOGCMN_CONTENT_H__
