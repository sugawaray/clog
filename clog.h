#ifndef __NCLOG_CLOG_H__
#define __NCLOG_CLOG_H__

#include <algorithm>
#include <functional>
#include <string>

namespace {
namespace clog {

using std::string;

struct Content {
	string message;
	union {
		int i;
	} return_value;

	Content(const string& message)
		:	message(message) {
		std::fill_n(reinterpret_cast<char*>(&return_value),
			sizeof return_value, 0);
	}
};

void outimpl(const Content&)
{
}

typedef void (*Out)(const Content&);
Out outfn(outimpl);

template<class F>
inline void out(const char* m, F f)
{
	f();
	outfn(Content(m));
}

template<class R>
inline R out(const char* m, R (f)())
{
	Content c(m);
	c.return_value.i = f();
	outfn(c);
	return c.return_value.i;
}

inline void out(const char* m, void (f)())
{
	f();
	outfn(Content(m));
}

template<class T>
inline void out(const char* m, void (f)(T), T a)
{
	auto bf(std::bind(f, a));
	out(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(T&), T& a)
{
	auto bf(std::bind(f, std::ref(a)));
	out(m, bf);
}

template<class T>
inline void out(const char* m, void (f)(const T&), const T& a)
{
	auto bf(std::bind(f, std::cref(a)));
	out(m, bf);
}

} // clog
} // unnamed

#endif // __NCLOG_CLOG_H__
