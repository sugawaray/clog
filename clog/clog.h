#ifndef __UNNAMED_CLOG_CLOG_H__
#define __UNNAMED_CLOG_CLOG_H__

#include "content.h"
#include <functional>

namespace {
namespace clog {

using clogcmn::Content;

void outimpl(const Content&)
{
}

typedef void (*Out)(const Content&);
Out outfn(outimpl);

template<class F>
inline void out_void(const char* m, F f)
{
	f();
	outfn(Content(m));
}

template<class R, class F>
inline R out(const char* m, F f)
{
	R r(f());
	outfn(Content(m));
	return r;
}

template<class R>
inline R out(const char* m, R (f)())
{
	R r(f());
	outfn(Content(m));
	return r;
}

inline void out(const char* m, void (f)())
{
	try {
		f();
		outfn(Content(m));
	}
	catch (...) {
		Content c(m);
		c.exception = true;
		outfn(c);
		throw;
	}
}

template<class T>
inline void out(const char* m, void (f)(T), T a)
{
	auto bf(std::bind(f, a));
	out_void(m, bf);
}

template<class R, class T>
inline R out(const char* m, R (f)(T), T a)
{
	auto bf(std::bind(f, a));
	return out<R>(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(T&), T& a)
{
	auto bf(std::bind(f, std::ref(a)));
	out_void(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(const T&), const T& a)
{
	auto bf(std::bind(f, std::cref(a)));
	out_void(m, bf);
}

} // clog
} // unnamed

#endif // __UNNAMED_CLOG_CLOG_H__
