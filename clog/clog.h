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

template<class T>
struct Arg {
	static T convert(T a) {
		return a;
	}
};

template<class T>
struct Arg<T&> {
	static std::reference_wrapper<T> convert(T& a) {
		return std::ref(a);
	}
};

template<class T>
struct Arg<const T&> {
	static std::reference_wrapper<const T> convert(const T& a) {
		return std::cref(a);
	}
};

template<class R, class F>
struct Outcall {
	static R call(const char* m, F f) {
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
};

template<class F>
struct Outcall<void, F> {
	static void call(const char* m, F f) {
		try {
			f();
			outfn(Content(m));
		}
		catch (...) {
			d::outex(m);
			throw;
		}
	}
};

} // d

template<class F>
class Outl {
};

template<class R>
class Outl<R(*)()> {
public:
	typedef R result_type;

	Outl(const char* m, R(*f)())
		:	f(f), m(m) {
	}

	R operator()() const {
		return d::Outcall<R, R(*)()>::call(m, f);
	}

private:
	R (*f)();
	const char* m;
};

template<class R, class T1>
class Outl<R(*)(T1)> {
public:
	typedef R result_type;

	Outl(const char* m, R (*f)(T1))
		:	f(f), m(m) {
	}

	R operator()(T1 a1) const {
		auto bf(std::bind(f, d::Arg<T1>::convert(a1)));
		return d::Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	typedef R (*F)(T1);
	F f;
	const char* m;
};

template<class R, class T1, class T2>
class Outl<R(*)(T1,T2)> {
public:
	typedef R result_type;

	Outl(const char* m, R (*f)(T1, T2))
		:	f(f), m(m) {
	}

	R operator()(T1 a1, T2 a2) const {
		auto bf(std::bind(f, d::Arg<T1>::convert(a1),
			d::Arg<T2>::convert(a2)));
		return d::Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	typedef R (*F)(T1, T2);
	F f;
	const char* m;
};

template<class T>
inline Outl<T> outl(const char* m, T f)
{
	return Outl<T>(m, f);
}

} // clog
} // unnamed

#endif // __UNNAMED_CLOG_CLOG_H__
