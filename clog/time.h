#ifndef __CLOGCMN_TIME_H__
#define __CLOGCMN_TIME_H__

#include <cstddef>
#include <ctime>

namespace clogcmn {

class Elapsed_time {
public:
	Elapsed_time();
	void start();
	bool now(long* buffer) const;
	static int (*clock_gettimefn)(clockid_t, struct timespec*);
private:
	static long nanos(const timespec& ts);
	timespec t;
	bool init_done;
};

} // clogcmn

#endif // __CLOGCMN_TIME_H__
