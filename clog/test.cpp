#include "spy.h"
#include "spy_fixture.h"
#include <clog/clog.h>
#include <clog/config.h>
#include <nomagic.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "macros.h"

namespace {

using nomagic::loc;
using nomagic::Test;

using clogcmn::Content;
using test::Spy;

namespace nlogcmn {

typedef Spy_fixture Fi;

namespace nf {

bool called;

class A {
public:
	A() : v1(0), v2(0) {
	}

	A(const A& op)
		:	v1(op.v1), v2(op.v2) {
		copied = true;
	}

	int v1;
	int v2;
	static bool copied;
};
bool A::copied;

A a;
A arg;

const int default_v1(1);
const int default_v2(2);

void reset()
{
	called = false;
	a = A();
	a.v1 = 1;
	a.v2 = 2;
	arg = A();
	A::copied = false;
}

struct E {
};

} // nf

} // nlogcmn

namespace nlog {

using nlogcmn::Fi;

namespace nf = nlogcmn::nf;

void f1()
{
	nf::called = true;
}

namespace d {

using std::enable_if;
using std::is_void;

template<class T>
struct enable_void : enable_if<is_void<T>::value> {
};

template<class T>
struct disable_void : enable_if<!is_void<T>::value> {
};

} // d

class Test_common {
public:
	Test_common(const char* ms)
		:	t(ms), m("message") {
	}

	void operator()() {
		Fi f;
		m = "message";
		nf::reset();
		prepare_call();
		call_and_assert();
		verify();
	}
protected:
	virtual void call_and_assert() = 0;
	virtual void prepare_call() {
	}
	virtual void verify() = 0;

	Test t;
	const char* m;
};

template<class F>
class Test_0 : public Test_common {
public:
	Test_0(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}
protected:
	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
	}

	F f;
private:
	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))();
	}
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))() == 1, L);
	}
};

class Extended_impl {
public:
	Extended_impl() {
		config_list = clogcmn::Config_list::create(configs);
		clog::config_list = &config_list;
		configs[0].message = "message";
		configs[0].measure_etime = false;
	}
protected:
	clogcmn::Config_list config_list;
	clogcmn::Config configs[1];
};

template<class F>
class Test_extended_0 : public Test_0<F>, private Extended_impl {
private:
	using Test_0<F>::m;
	using Test_0<F>::f;
	using Test_0<F>::t;
public:
	Test_extended_0(F f, const char* ms)
		:	Test_0<F>(f, ms) {
	}
protected:
	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}

private:
	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(0, f))();
	}
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(0, f))() == 1, L);
	}
};

using std::logic_error;

template<class F>
class Test_ex : public Test_common {
public:
	Test_ex(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}

protected:
	void call_and_assert() {
		try {
			call();
			logic_error("test");
		}
		catch (const nf::E&) {
		}
	}

	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(Spy::last()->exception, L);
	}

	virtual void call() = 0;

	F f;
};

template<class F>
class Test_0_ex : public Test_ex<F> {
public:
	Test_0_ex(F f, const char* ms)
		:	Test_ex<F>(f, ms) {
	}

protected:
	using Test_ex<F>::f;
	using Test_ex<F>::m;

	void call() {
		(clog::out(m, f))();
	}
};

template<class F>
class Test_1_ex : public Test_ex<F> {
public:
	Test_1_ex(F f, const char* ms)
		:	Test_ex<F>(f, ms) {
	}

protected:
	using Test_ex<F>::f;
	using Test_ex<F>::m;

	void call() {
		(clog::out(m, f))(1);
	}
};

template<class F>
inline Test_0_ex<F> test_0_ex(F f, const char* ms)
{
	return Test_0_ex<F>(f, ms);
}

template<class F>
inline Test_1_ex<F> test_1_ex(F f, const char* ms)
{
	return Test_1_ex<F>(f, ms);
}

template<class F>
inline Test_0<F> test_0(F f, const char* ms)
{
	return Test_0<F>(f, ms);
}

template<class F>
inline Test_extended_0<F> test_extended_0(F f, const char* ms)
{
	return Test_extended_0<F>(f, ms);
}

namespace nsimple {

void t1(const char* ms)
{
	(test_0(f1, ms))();
}

} // nsimple

namespace nextended {

void t1(const char* ms)
{
	(test_extended_0(f1, ms))();
}

} // nextended

void f2(int a)
{
	nf::called = true;
	nf::arg.v1 = a;
}

template<class F>
class Test_1 : public Test_common {
public:
	Test_1(F f, const char* ms) : Test_common(ms), f(f) {
	}

protected:
	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
		t.a(nf::arg.v1 == nf::a.v1, L);
		t.a(nf::A::copied == false, L);
	}

	F f;
};

template<class F>
class Test_2 : public Test_common {
public:
	Test_2(F f, const char* ms)
		:	Test_common(ms), f(f) {
	}

protected:
	void prepare_call() {
		nf::a.v1 = nf::default_v1 + 1;
		nf::a.v2 = nf::default_v2 + 1;
	}

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}

	void verify() {
		t.a(Spy::last()->message == m, L);
		t.a(nf::called, L);
		t.a(nf::arg.v1 == nf::a.v1, L);
		t.a(nf::arg.v2 == nf::a.v2, L);
	}
	F f;
private:
	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a.v1, nf::a.v2);
	}

	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a.v1, nf::a.v2) == 10, L);
	}

};

template<class F>
class Test_extended_2 : public Test_2<F>, private Extended_impl {
public:
	Test_extended_2(F f, const char* ms)
		:	Test_2<F>(f, ms) {
	}
protected:
	using Test_2<F>::f;
	using Test_2<F>::t;

	void call_and_assert() {
		(clog::out(0, f))(nf::a.v1, nf::a.v2);
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(0, f))(nf::a.v1, nf::a.v2) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(0, f))(nf::a.v1, nf::a.v2);
	}
};

template<class F>
class Test_1v : public Test_1<F> {
public:
	Test_1v(F f, const char* ms) : Test_1<F>(f, ms) {
	}

protected:
	using Test_1<F>::m;
	using Test_1<F>::t;
	using Test_1<F>::f;

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
	void prepare_call() {
		nf::a.v1 = 2;
		nf::a.v2 = nf::arg.v2;
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a.v1) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a.v1);
	}
};

template<class F>
class Test_extended_1v : public Test_1v<F>, private Extended_impl {
public:
	Test_extended_1v(F f, const char* ms)
		:	Test_1v<F>(f, ms) {
	}
protected:
	using Test_1v<F>::f;
	using Test_1v<F>::t;
	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(0, f))(nf::a.v1) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(0, f))(nf::a.v1);
	}
};

template<class F>
inline Test_1v<F> test_1v(F f, const char* ms)
{
	return Test_1v<F>(f, ms);
}

template<class F>
inline Test_extended_1v<F> test_extended_1v(F f, const char* ms)
{
	return Test_extended_1v<F>(f, ms);
}

namespace nsimple {

void t2(const char* ms)
{
	(test_1v(f2, ms))();
}

} // nsimple

namespace nextended {

void t2(const char* ms)
{
	(test_extended_1v(f2, ms))();
}

} // nextended

void f3(nf::A& a)
{
	nf::called = true;
	nf::arg = a;
}

void f3c(const nf::A& a)
{
	nf::called = true;
	nf::arg = a;
}

template<class F>
class Test_1r : public Test_1<F> {
public:
	Test_1r(F f, const char* ms) : Test_1<F>(f, ms) {
	}

protected:
	using Test_1<F>::m;
	using Test_1<F>::t;
	using Test_1<F>::f;

	void call_and_assert() {
		call<typename clog::Out_result<F>::type>();
	}
private:
	template<class T>
	void call(typename d::disable_void<T>::type* = 0) {
		t.a((clog::out(m, f))(nf::a) == 1, L);
	}

	template<class T>
	void call(typename d::enable_void<T>::type* = 0) {
		(clog::out(m, f))(nf::a);
	}
};

template<class F>
inline Test_1r<F> test_1r(F f, const char* ms)
{
	return Test_1r<F>(f, ms);
}

namespace nsimple {

void t3(const char* ms)
{
	(test_1r(f3, ms))();
}

} // nsimple

int f1_1r(nf::A& a)
{
	nf::called = true;
	nf::arg = a;
	return 1;
}

int f1_1rc(const nf::A& a)
{
	nf::called = true;
	nf::arg = a;
	return 1;
}

namespace nsimple {

void t7(const char* ms)
{
	(test_1r(f1_1r, ms))();
}

void t8(const char* ms)
{
	(test_1r(f1_1rc, ms))();
}

void t4(const char* ms)
{
	(test_1r(f3c, ms))();
}

} // nsimple

int f1_0()
{
	nf::called = true;
	return 1;
}

namespace nsimple {

void t5(const char* ms)
{
	(test_0(f1_0, ms))();
}

} // nsimple

namespace nextended {

void t5(const char* ms)
{
	(test_extended_0(f1_0, ms))();
}

} // nextended

int f1_1(int v)
{
	nf::called = true;
	nf::arg.v1 = v;
	return 1;
}

namespace nsimple {

void t6(const char* ms)
{
	(test_1v(f1_1, ms))();
}

} // nsimple

namespace nextended {

void t6(const char* ms)
{
	(test_extended_1v(f1_1, ms))();
}

} // nextended

void f0e()
{
	throw nf::E();
}

namespace nsimple {

void t9(const char* ms)
{
	(test_0_ex(f0e, ms))();
}

} // nsimple

int f0ei()
{
	throw nf::E();
}

namespace nsimple {

void t10(const char* ms)
{
	(test_0_ex(f0ei, ms))();
}

} // nsimple

void f1e(int a)
{
	throw nf::E();
}

int f1ei(int a)
{
	throw nf::E();
}

namespace nsimple {

void t11(const char* ms)
{
	(test_1_ex(f1e, ms))();
}

void t12(const char* ms)
{
	(test_1_ex(f1ei, ms))();
}

} // nsimple

template<class F>
inline Test_2<F> test_2(F f, const char* ms)
{
	return Test_2<F>(f, ms);
}

template<class F>
inline Test_extended_2<F> test_extended_2(F f, const char* ms)
{
	return Test_extended_2<F>(f, ms);
}

void fv2(int v1, int v2)
{
	nf::called = true;
	nf::arg.v1 = v1;
	nf::arg.v2 = v2;
}

namespace nsimple {

void t14(const char* ms)
{
	(test_2(fv2, ms))();
}

} // nsimple

namespace nextended {

void t14(const char* ms)
{
	(test_extended_2(fv2, ms))();
}

} // nextended

int fi2(int v1, int v2)
{
	nf::called = true;
	nf::arg.v1 = v1;
	nf::arg.v2 = v2;
	return 10;
}

namespace nsimple {

void t15(const char* ms)
{
	(test_2(fi2, ms))();
}

} // nsimple

} // nlog

namespace nlogm {

using nlogcmn::Fi;
namespace nf = nlogcmn::nf;

class Sample {
public:
	Sample()
		:	called(false) {
	}

	bool is_called() const {
		return called;
	}

	void mv0() {
		receive_call();
	}

	void mv0c() const {
		receive_call();
	}

	void mv1(nf::A& a) {
		receive_call();
		nf::arg = a;
	}

	void mv1c(nf::A& a) const {
		receive_call();
		nf::arg = a;
	}

	int mi0() {
		receive_call();
		return 1;
	}

	int mi0c() const {
		receive_call();
		return 1;
	}
private:
	void receive_call() const {
		called = true;
	}

	mutable bool called;
};

class Test_0 : protected Test, private Fi {
public:
	Test_0(const char* ms)
		:	Test(ms), m("message") {
	}

	void start() {
		Spy::reset();
		nf::reset();
		call_and_assert();
		a(Spy::last()->message == m, L);
		a(sample.is_called(), L);
		verify();
	}
protected:
	virtual void call_and_assert() = 0;
	virtual void verify() {
	}

	const char* m;
	Sample sample;
};

class Test_v0 : public Test_0 {
public:
	Test_v0(const char* ms)
		:	Test_0(ms) {
	}
protected:
	void call_and_assert() {
		(clog::out(m, &Sample::mv0))(sample);
	}
};

class Test_v0c : public Test_0 {
public:
	Test_v0c(const char* ms)
		:	Test_0(ms) {
	}
protected:
	void call_and_assert() {
		(clog::out(m, &Sample::mv0c))(sample);
	}
};

class Test_1 : public Test_0 {
public:
	Test_1(const char* ms)
		:	Test_0(ms) {
	}
protected:
	void verify() {
		a(nf::arg.v1 == nf::a.v1, L);
		a(!nf::A::copied, L);
	}
};

class Test_v1 : public Test_1 {
public:
	Test_v1(const char* ms)
		:	Test_1(ms) {
	}
protected:
	void call_and_assert() {
		(clog::out(m, &Sample::mv1))(sample, nf::a);
	}
};

class Test_v1c : public Test_1 {
public:
	Test_v1c(const char* ms)
		:	Test_1(ms) {
	}
protected:
	void call_and_assert() {
		(clog::out(m, &Sample::mv1c))(sample, nf::a);
	}
};

class Test_i0 : public Test_0 {
public:
	Test_i0(const char* ms)
		:	Test_0(ms) {
	}
protected:
	void call_and_assert() {
		a((clog::out(m, &Sample::mi0))(sample) == 1, L);
	}
};

class Test_i0c : public Test_0 {
public:
	Test_i0c(const char* ms)
		:	Test_0(ms) {
	}
protected:
	void call_and_assert() {
		a((clog::out(m, &Sample::mi0c))(sample) == 1, L);
	}
};

void t1(const char* ms)
{
	(Test_v0(ms)).start();
}

void t2(const char* ms)
{
	(Test_i0(ms)).start();
}

void t3(const char* ms)
{
	(Test_v0c(ms)).start();
}

void t4(const char* ms)
{
	(Test_i0c(ms)).start();
}

void t5(const char* ms)
{
	(Test_v1(ms)).start();
}

void t6(const char* ms)
{
	(Test_v1c(ms)).start();
}

} // nlogm

using nomagic::run;

void log_tests()
{
	std::cerr << "simple log tests" << std::endl;

	using namespace nlog::nsimple;

	run("return(void), arg(0)", t1);
	run("return(void), arg(1)", t2);
	run("return(void), arg(1, ref)", t3);
	run("return(void), arg(1, cref)", t4);
	run("return(int), arg(0)", t5);
	run("return(int), arg(1)", t6);
	run("return(int), arg(1, ref)", t7);
	run("return(int), arg(1, cref)", t8);
	run("return(void), arg(0), exception", t9);
	run("return(int), arg(0), exception", t10);
	run("return(void), arg(1)", t11);
	run("return(int), arg(1)", t12);
	run("return(void), arg(2)", t14);
	run("return(int), arg(2)", t15);
}

void log_extended_tests()
{
	std::cerr << "extended log tests" << std::endl;

	using namespace nlog::nextended;

	run("return(void), arg(0)", t1);
	run("return(void), arg(1)", t2);
	run("return(int), arg(0)", t5);
	run("return(int), arg(1)", t6);
	run("return(void), arg(2)", t14);
}

void log_method_tests()
{
	using namespace nlogm;

	run("method, return(void), arg(0)", t1);
	run("method, return(int), arg(0)", t2);
	run("method, return(void), arg(0), const", t3);
	run("method, return(int), arg(0), const", t4);
	run("method, return(void), arg(1)", t5);
	run("method, return(void), arg(1), const", t6);
}

} // unnamed

#include "test.h"

namespace test {

void run_clog_tests()
{
	log_tests();
	log_extended_tests();
	log_method_tests();
}

} // test

