
#include <string_view>
#include <iostream>

#include "ow/utils.hpp"

std::ostream& ow::logger_impl(std::string_view file, std::string_view function,
					  unsigned long line) {
	return std::cout << '[' << file << ':' << line << " (" << function << ")]:";
}