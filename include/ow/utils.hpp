#pragma once

#include <string_view>

namespace ow {
#define logger logger_impl(__FILE__, __FUNCTION__, __LINE__)

	std::ostream& logger_impl(std::string_view, std::string_view, unsigned long);
}
