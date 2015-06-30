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

namespace d {

inline void outex(const char* m)
{
	Content c(m);
	c.exception = true;
	outfn(c);
}

} // d

template<class F>
inline void out_void(const char* m, F f)
{
	try {
		f();
		outfn(Content(m));
	}
	catch (...) {
		d::outex(m);
		throw;
	}
}

template<class R, class F>
inline R out(const char* m, F f)
{
	try {
		R r(f());
		outfn(Content(m));
		return r;
	}
	catch (...) {
		d::outex(m);
		throw;
	}
}

template<class R>
inline R out(const char* m, R (f)())
{
	try {
		R r(f());
		outfn(Content(m));
		return r;
	}
	catch (...) {
		d::outex(m);
		throw;
	}
}

inline void out(const char* m, void (f)())
{
	try {
		f();
		outfn(Content(m));
	}
	catch (...) {
		d::outex(m);
		throw;
	}
}

template<class T>
inline void out(const char* m, void (f)(T), T a)
{
	auto bf(std::bind(f, a));
	out_void(m, bf);
}

template<class T1, class T2>
inline void out(const char* m, void (f)(T1, T2), T1 v1, T2 v2)
{
	auto bf(std::bind(f, v1, v2));
	out_void(m, bf);
}

template<class R, class T>
inline R out(const char* m, R (f)(T), T a)
{
	auto bf(std::bind(f, a));
	return out<R>(m, bf);
}

template<class R, class T1, class T2>
inline R out(const char* m, R(f)(T1, T2), T1 v1, T2 v2)
{
	auto bf(std::bind(f, v1, v2));
	return out<R>(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(T&), T& a)
{
	auto bf(std::bind(f, std::ref(a)));
	out_void(m, bf);
}

template<class R, class T>
inline R out(const char* m, R(f)(T&), T& a)
{
	auto bf(std::bind(f, std::ref(a)));
	return out<R>(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(const T&), const T& a)
{
	auto bf(std::bind(f, std::cref(a)));
	out_void(m, bf);
}

template<class R, class T>
inline R out(const char* m, R(f)(const T&), const T& a)
{
	auto bf(std::bind(f, std::cref(a)));
	return out<R>(m, bf);
}

} // clog
} // unnamed

#endif // __UNNAMED_CLOG_CLOG_H__
