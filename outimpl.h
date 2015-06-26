#ifndef __CLOG_OUTIMPL_H__
#define __CLOG_OUTIMPL_H__

#include <ostream>
#include <string>

namespace clog {
	struct Content;

extern std::ostream* stream;

extern void outimpl(const Content& content);
extern std::string to_string(const Content& content);

} // clog

#endif // __CLOG_OUTIMPL_H__
