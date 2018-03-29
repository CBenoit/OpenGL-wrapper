#ifndef IN55_EXCEPTIONS_HPP
#define IN55_EXCEPTIONS_HPP

#include <stdexcept>

namespace ow {
    class invalid_state : public std::logic_error {
    public:
        explicit invalid_state(const char *what_arg) : std::logic_error(what_arg) {}

        explicit invalid_state(const std::string &what_arg) : std::logic_error(what_arg) {}
    };
}

#endif //IN55_EXCEPTIONS_HPP
