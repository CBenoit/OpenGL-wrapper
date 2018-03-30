#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <tuple>
#include <vector>
#include "checkable.hpp"
#include "opengl_codes.hpp"

namespace ow {

class shader_program : public checkable {
public:
    shader_program() : checkable{}, m_program_id{glCreateProgram()} {
        p_state = (get_id() != 0);
    }

    explicit shader_program(const std::vector<std::pair<GLenum, std::string_view>>& shaders) : shader_program() {
        put(shaders);
    }

    shader_program(const shader_program&) = delete;

    shader_program(shader_program&&) noexcept ;

    ~shader_program() {
        if (get_id() != 0) {
            glDeleteProgram(m_program_id);
        }
    }

    bool put(const std::vector<std::pair<GLenum, std::string_view>>& shaders);

    // get the OpenGL program id
    GLuint get_id() const noexcept {
        return m_program_id;
    }

    // use the shader program
    void use() const {
        chk_state();
        glUseProgram(get_id());
        check_errors("Error while setting " + std::to_string(get_id()) + " as shader program.\n");
    }

    template <typename T>
    void set(std::string_view name, T value) const noexcept {
        using namespace std;
        using namespace glm;

        if constexpr (is_same_v<bool, T>) {
            glUniform1i(glGetUniformLocation(m_program_id, name.data()), value ? GL_TRUE : GL_FALSE);

        } else if constexpr (is_same_v<int, T>) {
            glUniform1i(glGetUniformLocation(m_program_id, name.data()), value);

        } else if constexpr (is_same_v<float, T>) {
            glUniform1f(glGetUniformLocation(m_program_id, name.data()), value);

        } else if constexpr (is_same_v<unsigned int, T> || is_same_v<unsigned long, T>) {
            glUniform1ui(glGetUniformLocation(m_program_id, name.data()), static_cast<unsigned int>(value));

        } else if constexpr (is_same_v<tuple<float, float, float, float>, T>) {
            glUniform4f(glGetUniformLocation(m_program_id, name.data()),
                        get<0>(value),
                        get<1>(value),
                        get<2>(value),
                        get<3>(value)
            );

        } else if constexpr (is_same_v<tuple<float, float, float>, T>) {
            glUniform3f(glGetUniformLocation(m_program_id, name.data()),
                        get<0>(value),
                        get<1>(value),
                        get<2>(value)
            );

        } else if constexpr (is_same_v<tuple<float, float>, T>) {
            glUniform2f(glGetUniformLocation(m_program_id, name.data()),
                        get<0>(value),
                        get<1>(value)
            );

        } else if constexpr (is_same_v<vec4, T>) {
            glUniform4fv(glGetUniformLocation(m_program_id, name.data()), 1, value_ptr(value));

        } else if constexpr (is_same_v<vec3, T>) {
            glUniform3fv(glGetUniformLocation(m_program_id, name.data()), 1, value_ptr(value));

        } else if constexpr (is_same_v<vec2, T>) {
            glUniform2fv(glGetUniformLocation(m_program_id, name.data()), 1, value_ptr(value));

        } else if constexpr (is_same_v<mat4, T>) {
            glUniformMatrix4fv(glGetUniformLocation(m_program_id, name.data()), 1, GL_FALSE, value_ptr(value));

        } else if constexpr (is_same_v<mat3, T>) {
            glUniformMatrix3fv(glGetUniformLocation(m_program_id, name.data()), 1, GL_FALSE, value_ptr(value));

        } else if constexpr (is_same_v<mat2, T>) {
            glUniformMatrix2fv(glGetUniformLocation(m_program_id, name.data()), 1, GL_FALSE, value_ptr(value));

        } else {
            static_assert(is_same_v<T,T*>, "Unknown type");
        }
    }


private:
    static bool load_shader(GLuint shader, std::string_view file_name);

    static bool checked_compile(GLuint shader, std::string_view shader_file);

    static bool load_compile(GLuint shader, std::string_view shader_file);

private:
    GLuint m_program_id;
};

}

