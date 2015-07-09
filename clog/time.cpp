#include "time.h"

namespace clogcmn {

int (*Elapsed_time::clock_gettimefn)(clockid_t, struct timespec*)(
	clock_gettime);

Elapsed_time::Elapsed_time()
:	init_done(false)
{
}

void Elapsed_time::start()
{
	clock_gettimefn(CLOCK_REALTIME, &t);
	init_done = true;
}

inline long Elapsed_time::nanos(const timespec& t)
{
	return t.tv_sec * 1000000 + t.tv_nsec;
}

bool Elapsed_time::now(long* buffer) const
{
	if (buffer == 0)
		return false;
	timespec n;
	clock_gettimefn(CLOCK_REALTIME, &n);
	*buffer = nanos(n) - nanos(t);
	return init_done;
}

} // clogcmn
