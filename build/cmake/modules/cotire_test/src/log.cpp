// cotire example project

#include "log.h"

#include <iostream>

namespace logging {

void error(const std::string& msg) {
	std::cerr << msg << std::endl;
}

void info(const std::string& msg) {
	std::cout << msg << std::endl;
}

}
