#ifndef __UNNAMED_CLOG_CLOG_H__
#define __UNNAMED_CLOG_CLOG_H__

#include "config.h"
#include "content.h"
#include "time.h"
#include <functional>

namespace {
namespace clog {

using clogcmn::Config;
using clogcmn::Config_list;
using clogcmn::Content;
using clogcmn::Elapsed_time;

void outimpl(const Content&)
{
}

typedef void (*Out)(const Content&);
Out outfn(outimpl);

Config_list* config_list(0);

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

inline const clogcmn::Config& config_at(int i)
{
	return (*config_list)[i];
}

template<class R, class F>
struct Implbuf;

template<class R, class F>
struct Implbase {
	virtual ~Implbase() throw() {
	}

	virtual R run(int i, F f) = 0;
};

template<class T>
struct Impl_type {
};
template<template<class A, class B> class Te, class R, class F>
struct Impl_type<Te<R, F> > {
	typedef R result_type;
	typedef F func_type;
};

template<class D>
class Impl :
	public Implbase<typename Impl_type<D>::result_type,
		typename Impl_type<D>::func_type> {
public:
	typedef D derived;
	typedef typename Impl_type<D>::result_type result_type;
	typedef typename Impl_type<D>::func_type func_type;

	result_type run(int i, func_type f) {
		return runimpl<result_type>(i, f);
	}

private:
	template<class T>
	typename std::enable_if<std::is_void<T>::value>::type
		runimpl(int i, func_type f) {
		derived* p(static_cast<derived*>(this));
		p->prefunc(i);
		try {
			f();
			p->postfunc(i);
		}
		catch (...) {
			if (config_list != 0)
				outex(((*config_list)[i]).message);
			throw;
		}
	}

	template<class T>
	typename std::enable_if<
		!std::is_void<T>::value, result_type>::type
		runimpl(int i, func_type f) {
		derived* p(static_cast<derived*>(this));
		p->prefunc(i);
		try {
			result_type r(f());
			p->postfunc(i);
			return r;
		}
		catch (...) {
			if (config_list != 0)
				outex(((*config_list)[i]).message);
			throw;
		}
	}
};

template<class R, class F>
class Impl_without_list : public Impl<Impl_without_list<R, F> > {
public:
	typedef R result_type;
	typedef F func_type;

	Impl_without_list(Implbuf<R, F>*) {
	}

	void prefunc(int i) {
	}

	void postfunc(int i) {
	}
};

template<class R, class F>
class Impl_basic : public Impl<Impl_basic<R, F> > {
public:
	typedef R result_type;
	typedef F func_type;

	Impl_basic(Implbuf<R, F>* buf)
		:	b(buf) {
	}

	void prefunc(int i) {
		if (config_at(i).measure_etime) {
			b->c.elapsed_time_valid = true;
			b->time.start();
		}
	}

	void postfunc(int i) {
		if (config_at(i).measure_etime)
			b->time.now(&b->c.elapsed_clocks);
		b->c.message = config_at(i).message;
		outfn(b->c);
	}
private:
	Implbuf<R, F>* b;
};

template<class R, class F>
class Implbuf {
public:
	Content c;
	Elapsed_time time;

	Implbase<R, F>* get_impl() {
		if (config_list == 0)
			return &impl_without_list;
		else
			return &impl_basic;
	}

	Implbuf()
		:	impl_without_list(this),
			impl_basic(this) {
	}
private:
	Impl_without_list<R, F> impl_without_list;
	Impl_basic<R, F> impl_basic;
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
			outex(m);
			throw;
		}
	}
	static R call(int i, F f) {
		Implbuf<R, F> impl;
		return impl.get_impl()->run(i, f);
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
			outex(m);
			throw;
		}
	}
	static void call(int i, F f) {
		Implbuf<void, F> impl;
		return impl.get_impl()->run(i, f);
	}
};

template<class F>
class Simple_fimpl {
};

template<class T>
class Fimpl {
};

template<template<class A> class Te, class R>
class Fimpl<Te<R(*)()> > {
public:
	typedef R result_type;
	typedef Te<R(*)()> derived;

	Fimpl(R(*f)()) : f(f) {
	}

	R operator()() const {
		return Outcall<R, R(*)()>::call(
			static_cast<const derived*>(this)->get_id(), f);
	}
private:
	R (*f)();
};

template<class R>
class Simple_fimpl<R(*)()> : public Fimpl<Simple_fimpl<R(*)()> > {
public:
	typedef R result_type;

	Simple_fimpl(const char* m, R(*f)())
		:	Fimpl<Simple_fimpl<R(*)()> >(f), m(m) {
	}

	const char* get_id() const {
		return m;
	}

private:
	const char* m;
};

template<class F>
class Extended_fimpl {
};

template<class R>
class Extended_fimpl<R(*)()> : public Fimpl<Extended_fimpl<R(*)()> > {
public:
	Extended_fimpl(int id, R(*f)())
		:	Fimpl<Extended_fimpl<R(*)()> >(f), id(id) {
	}

	int get_id() const {
		return id;
	}
private:
	int id;
};

template<template<class A> class Te, class R, class T1>
class Fimpl<Te<R(*)(T1)> > {
public:
	typedef R result_type;
	typedef Te<R(*)(T1)> derived;

	Fimpl(R (*f)(T1))
		:	f(f) {
	}

	R operator()(T1 a1) const {
		auto bf(std::bind(f, Arg<T1>::convert(a1)));
		return Outcall<R, decltype(bf)>::call(
			static_cast<const derived*>(this)->get_id(), bf);
	}
private:
	typedef R (*F)(T1);
	F f;
};

template<class R, class T1>
class Simple_fimpl<R(*)(T1)> : public Fimpl<Simple_fimpl<R(*)(T1)> > {
public:
	Simple_fimpl(const char* m, R (*f)(T1))
		:	Fimpl<Simple_fimpl<R(*)(T1)> >(f), m(m) {
	}

	const char* get_id() const {
		return m;
	}
private:
	const char* m;
};

template<class R, class T1>
class Extended_fimpl<R(*)(T1)> : public Fimpl<Extended_fimpl<R(*)(T1)> > {
public:
	Extended_fimpl(int id, R(*f)(T1))
		:	Fimpl<Extended_fimpl<R(*)(T1)> >(f), id(id) {
	}

	int get_id() const {
		return id;
	}
private:
	int id;
};

template<template<class A> class Te, class R, class T1, class T2>
class Fimpl<Te<R(*)(T1,T2)> > {
public:
	typedef R result_type;
	typedef Te<R(*)(T1,T2)> derived;

	Fimpl(R (*f)(T1, T2))
		:	f(f) {
	}

	R operator()(T1 a1, T2 a2) const {
		auto bf(std::bind(f, Arg<T1>::convert(a1),
			Arg<T2>::convert(a2)));
		return Outcall<R, decltype(bf)>::call(
			static_cast<const derived*>(this)->get_id(), bf);
	}
private:
	typedef R (*F)(T1, T2);
	F f;
};

template<class R, class T1, class T2>
class Simple_fimpl<R(*)(T1,T2)> : public Fimpl<Simple_fimpl<R(*)(T1,T2)> > {
public:
	typedef R result_type;

	Simple_fimpl(const char* m, R (*f)(T1, T2))
		:	Fimpl<Simple_fimpl<R(*)(T1,T2)> >(f), m(m) {
	}

	const char* get_id() const {
		return m;
	}
private:
	const char* m;
};

template<class R, class T1, class T2>
class Extended_fimpl<R(*)(T1,T2)> :
	public Fimpl<Extended_fimpl<R(*)(T1,T2)> > {
public:
	Extended_fimpl(int id, R(*f)(T1,T2))
		:	Fimpl<Extended_fimpl<R(*)(T1,T2)> >(f), id(id) {
	}

	int get_id() const {
		return id;
	}
private:
	int id;
};

} // d

namespace d {

template<class T, class R>
struct Enable_if_not_mp :
	std::enable_if<!std::is_member_pointer<T>::value, R> {
};

template<class T, class R>
struct Enable_if_mp :
	std::enable_if<std::is_member_pointer<T>::value, R> {
};

template<class T>
class Cimpl {
public:
};

template<class C, class R>
class Cimpl<R(C::*)()> {
public:
	Cimpl(const char* m, R(C::*p)())
		:	m(m), p(p) {
	}

	R operator()(C& o) const {
		auto bf(std::bind(p, Arg<C&>::convert(o)));
		return Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	const char* m;
	R (C::*p)();
};

template<class C, class R>
class Cimpl<R(C::*)() const> {
public:
	Cimpl(const char* m, R(C::*p)() const)
		:	m(m), p(p) {
	}

	R operator()(const C& o) const {
		auto bf(std::bind(p, Arg<const C&>::convert(o)));
		return Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	const char* m;
	R (C::*p)() const;
};

template<class C, class R, class A1>
class Cimpl<R(C::*)(A1)> {
public:
	Cimpl(const char* m, R(C::*p)(A1))
		:	m(m), p(p) {
	}

	R operator()(C& o, A1 a1) const {
		auto bf(std::bind(p, Arg<C&>::convert(o),
			Arg<A1>::convert(a1)));
		return Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	const char* m;
	R (C::*p)(A1);
};

template<class C, class R, class A1>
class Cimpl<R(C::*)(A1) const> {
public:
	Cimpl(const char* m, R(C::*p)(A1) const)
		:	m(m), p(p) {
	}

	R operator()(C& o, A1 a1) const {
		auto bf(std::bind(p, Arg<const C&>::convert(o),
			Arg<A1>::convert(a1)));
		return Outcall<R, decltype(bf)>::call(m, bf);
	}
private:
	const char* m;
	R (C::*p)(A1) const;
};

} // d

template<class T>
inline typename d::Enable_if_not_mp<T, d::Simple_fimpl<T> >::type
	out(const char* m, T f)
{
	return d::Simple_fimpl<T>(m, f);
}

template<class T>
inline typename d::Enable_if_not_mp<T, d::Extended_fimpl<T> >::type
	out(int id, T f)
{
	return d::Extended_fimpl<T>(id, f);
}

template<class T>
inline typename d::Enable_if_mp<T, d::Cimpl<T> >::type
	out(const char* m, T mp)
{
	return d::Cimpl<T>(m, mp);
}

template<class F>
struct Out_result {
	typedef typename d::Simple_fimpl<F>::result_type type;
};

} // clog
} // unnamed

#endif // __UNNAMED_CLOG_CLOG_H__
