#ifndef __CONFIG_LIST_FIXTURE_H__
#define __CONFIG_LIST_FIXTURE_H__

#include "clog.h"
#include "config.h"

namespace test {
namespace {

using clog::config_list;
using clogcmn::Config;
using clogcmn::Config_list;

template<int N>
class Config_list_fixture {
public:
	Config_list_fixture() {
		list = Config_list::create(configs);
		config_list = &list;
	}

	Config (&get_configs())[N] {
		return configs;
	}
private:
	Config configs[N];
	Config_list list;
};

} // unnamed
} // test

#endif // __CONFIG_LIST_FIXTURE_H__
