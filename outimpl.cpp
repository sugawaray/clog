#include "content.h"
#include "outimpl.h"
#include <iostream>
#include <sstream>

namespace clog {

std::ostream* stream(&std::cout);

void outimpl(const Content& content)
{
	(*stream) << content.message << std::endl;
}

std::string to_string(const Content& content)
{
	if (!content.exception)
		return content.message;
	else {
		std::ostringstream os;
		os << content.message << "(an exception is thrown)";
		return os.str();
	}
}

} // clog
