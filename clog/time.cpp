#include "time.h"

namespace clogcmn {

std::clock_t (*Elapsed_time::clockfn)()(std::clock);

Elapsed_time::Elapsed_time()
:	c(0), init_done(false)
{
}

void Elapsed_time::start()
{
	c = clockfn();
	init_done = true;
}

bool Elapsed_time::now(std::size_t* buffer) const
{
	if (buffer == 0)
		return false;
	*buffer = clockfn() - c;
	return init_done;
}

} // clogcmn
