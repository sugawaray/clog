#ifndef __CLOGCMN_CONFIG_H__
#define __CLOGCMN_CONFIG_H__

#include <cstddef>

namespace clogcmn {

struct Config {
	const char* message;
	bool measure_etime;
	bool store_return_value;
};

class Config_list {
public:
	template<std::size_t N>
	static Config_list create(Config (&list)[N]) {
		Config_list r;
		r.list = list;
		return r;
	}

	Config_list() : list(0) {
	}

	const Config& operator[](std::size_t index) const {
		return list[index];
	}
private:

	Config* list;
};

} // clogcmn

#endif // __CLOGCMN_CONFIG_H__
