#include "outimpl.h"
#include <clog/content.h>
#include <ctime>
#include <iostream>
#include <sstream>

namespace impl {

std::ostream* stream(&std::cout);

void outimpl(const Content& content)
{
	(*stream) << to_string(content) << std::endl;
}

class Nanos {
public:
	Nanos(long v)
		:	v(v) {
	}

	long v;
};

std::ostream& operator<<(std::ostream& os, const Nanos& n)
{
	os << "(duration:" << n.v / 1000000.0 << ")";
	return os;
}

std::string to_string(const Content& content)
{
	std::ostringstream os;
	os << content.message;
	if (content.exception)
		os << "(an exception is thrown)";
	if (content.elapsed_time_valid)
		os << Nanos(content.elapsed_time);
	return os.str();
}

} // impl
