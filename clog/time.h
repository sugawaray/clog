#ifndef __CLOGCMN_TIME_H__
#define __CLOGCMN_TIME_H__

#include <cstddef>
#include <ctime>

namespace clogcmn {

class Elapsed_time {
public:
	void start();
	std::size_t now() const;
	static std::clock_t (*clockfn)();
private:
	std::clock_t c;
};

} // clogcmn

#endif // __CLOGCMN_TIME_H__
