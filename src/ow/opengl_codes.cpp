
#include <ow/opengl_codes.hpp>
#include <iostream>
#include <fstream>
#include <ow/utils.hpp>

namespace ow {

    template<>
    GLenum get_gl_type<GLbyte>() {
        return GL_BYTE;
    }

    template<>
    GLenum get_gl_type<GLubyte>() {
        return GL_UNSIGNED_BYTE;
    }

    template<>
    GLenum get_gl_type<GLshort>() {
        return GL_SHORT;
    }

    template<>
    GLenum get_gl_type<GLushort>() {
        return GL_UNSIGNED_SHORT;
    }

    template<>
    GLenum get_gl_type<GLint>() {
        return GL_INT;
    }

    template<>
    GLenum get_gl_type<GLuint>() {
        return GL_UNSIGNED_INT;
    }

    template<>
    GLenum get_gl_type<GLfloat>() {
        return GL_FLOAT;
    }

    template<>
    GLenum get_gl_type<GLdouble>() {
        return GL_DOUBLE;
    }

    bool check_errors_impl(std::string_view error_message, std::string_view file, std::string_view function,
                      unsigned long line) {

        auto ec = glGetError();
        if (ec != GL_NO_ERROR) {
	        logger_impl(file, function, line) << error_message.data() << "ec: " << ec_to_string(ec) << '\n';
            return false;
        }
        return true;
    }

    std::string ec_to_string(GLenum error_code) {

        switch (error_code) {
            case GL_NO_ERROR:
                return "GL_NO_ERROR";
            case GL_INVALID_ENUM:
                return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE:
                return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION:
                return "GL_INVALID_OPERATION";
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "GL_INVALID_FRAMEBUFFER_OPERATION";
            case GL_OUT_OF_MEMORY:
                return "GL_OUT_OF_MEMORY";
            default:
                return "Unknown error (" + std::to_string(error_code) + ")";
        }
    }

}