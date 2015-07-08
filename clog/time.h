#ifndef __CLOGCMN_TIME_H__
#define __CLOGCMN_TIME_H__

#include <cstddef>
#include <ctime>

namespace clogcmn {

class Elapsed_time {
public:
	Elapsed_time();
	void start();
	bool now(std::size_t* buffer) const;
	static std::clock_t (*clockfn)();
private:
	std::clock_t c;
	bool init_done;
};

} // clogcmn

#endif // __CLOGCMN_TIME_H__
