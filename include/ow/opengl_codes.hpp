#pragma once

#include <glad/glad.h>
#include <string>

namespace ow {

    template<typename T>
    GLenum get_gl_type();

    std::string ec_to_string(GLenum error_code);

#define check_errors(error_message) check_errors_impl(error_message, __FILE__, __FUNCTION__, __LINE__)

    bool check_errors_impl(std::string_view error_message, std::string_view, std::string_view, unsigned long);
}
