#ifndef __CLOGCMN_CONTENT_H__
#define __CLOGCMN_CONTENT_H__

#include <cstddef>
#include <sstream>
#include <string>

namespace clogcmn {

using std::string;

const char* const Notset("notset");

class Var {
	enum {
		Tc, Tuc, Tsi, Tusi, Ti, Tui, Tli, Tuli, Tlli, Tulli
	};
public:
	Var()
		:	fset(false) {
	}

	std::string to_string() const {
		if (fset) {
			std::stringstream s;
			write(s);
			return s.str();
		} else
			return Notset;
	}

	void set(char newone) {
		setvar(uv.c, newone);
	}

	void set(unsigned char newone) {
		setvar(uv.uc, newone);
	}

	void set(short newone) {
		setvar(uv.si, newone);
	}

	void set(unsigned short newone) {
		setvar(uv.usi, newone);
	}

	void set(int newone) {
		setvar(uv.i, newone);
	}

	void set(unsigned int newone) {
		setvar(uv.ui, newone);
	}

	void set(long newone) {
		setvar(uv.li, newone);
	}

	void set(unsigned long newone) {
		setvar(uv.uli, newone);
	}

	void set(long long int newone) {
		setvar(uv.lli, newone);
	}

	void set(unsigned long long int newone) {
		setvar(uv.ulli, newone);
	}
private:
	template<class T>
	void setvar(T& var, T value) {
		var = value;
		fset = true;
		settype(value);
	}

	void settype(char) {
		type = Tc;
	}

	void settype(unsigned char) {
		type = Tuc;
	}

	void settype(short) {
		type = Tsi;
	}

	void settype(unsigned short) {
		type = Tusi;
	}

	void settype(int) {
		type = Ti;
	}

	void settype(unsigned int) {
		type = Tui;
	}

	void settype(long) {
		type = Tli;
	}

	void settype(unsigned long) {
		type = Tuli;
	}

	void settype(long long int) {
		type = Tlli;
	}

	void settype(unsigned long long) {
		type = Tulli;
	}

	void write(std::stringstream& ss) const {
		switch (type) {
		case Tc:
			ss << static_cast<int>(uv.c);
			break;
		case Tuc:
			ss << static_cast<int>(uv.uc);
			break;
		case Tsi:
			ss << uv.si;
			break;
		case Tusi:
			ss << uv.usi;
			break;
		case Ti:
			ss << uv.i;
			break;
		case Tui:
			ss << uv.ui;
			break;
		case Tli:
			ss << uv.li;
			break;
		case Tuli:
			ss << uv.uli;
			break;
		case Tlli:
			ss << uv.lli;
			break;
		case Tulli:
			ss << uv.ulli;
			break;
		}
	}

	union U {
		char c;
		unsigned char uc;
		short si;
		unsigned short usi;
		int i;
		unsigned ui;
		long li;
		unsigned long uli;
		long long lli;
		unsigned long long ulli;
	};
	bool fset;
	U uv;
	int type;
};

struct Content {
	string message;
	bool exception;
	long elapsed_time;
	Var return_value;

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
