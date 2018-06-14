#pragma once

#include <glad/glad.h>
#include <string>

#include "ow/vertex.hpp"

namespace ow {

template<typename T>
GLenum get_gl_type() {
	// default implementation should fail.
	static_assert(std::is_same_v<T,T*>, "Unknown type"); // always fail.
	return get_gl_type<T>(); // just to return something.
}

std::string ec_to_string(GLenum error_code);

#ifndef NDEBUG
#define check_errors(error_message) check_errors_impl(error_message, __FILE__, __FUNCTION__, __LINE__)
#else
#define check_errors(error_message) static_cast<void*>(0)
#endif

bool check_errors_impl(std::string_view error_message, std::string_view, std::string_view, unsigned long);

}


namespace ow {

	template<>
	GLenum get_gl_type<vertex>();

	template<>
	GLenum get_gl_type<GLbyte>();

	template<>
	GLenum get_gl_type<GLubyte>();

	template<>
	GLenum get_gl_type<GLshort>();

	template<>
	GLenum get_gl_type<GLushort>();

	template<>
	GLenum get_gl_type<GLint>();

	template<>
	GLenum get_gl_type<GLuint>();

	template<>
	GLenum get_gl_type<GLfloat>();

	template<>
	GLenum get_gl_type<GLdouble>();
}
