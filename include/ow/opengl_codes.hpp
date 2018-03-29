#ifndef OW_OPENGL_CODES_HPP
#define OW_OPENGL_CODES_HPP

#include <glad/glad.h>
#include <string>

namespace ow {

    template<typename T>
    GLenum get_gl_type();

    std::string ec_to_string(GLenum error_code);

    bool check_errors(std::string_view error_message);
}

#endif //OW_OPENGL_CODES_HPP
