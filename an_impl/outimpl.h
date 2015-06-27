#ifndef __IMPL_OUTIMPL_H__
#define __IMPL_OUTIMPL_H__

#include <ostream>
#include <string>

namespace clogcmn {
	struct Content;
} // clogcmn

namespace impl {

using clogcmn::Content;

extern std::ostream* stream;

extern void outimpl(const Content& content);
extern std::string to_string(const Content& content);

} // impl

#endif // __IMPL_OUTIMPL_H__
