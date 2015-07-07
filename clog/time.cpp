#include "time.h"

namespace clogcmn {

std::clock_t (*Elapsed_time::clockfn)()(std::clock);

void Elapsed_time::start()
{
	c = clockfn();
}

std::size_t Elapsed_time::now() const
{
	return clockfn() - c;
}

} // clogcmn
