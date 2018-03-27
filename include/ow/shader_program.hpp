#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace ow {

class shader_program
{
public:
    // constructor reads and builds the shader
    shader_program(std::string vertex_file_path, std::string fragment_file_path);

    // get the OpenGL program id
    GLuint get_id() const {
        return m_program_id;
    }

    // use the shader program
    void use() const {
        glUseProgram(m_program_id);
    }

    // utility uniform functions
    void set_bool(const std::string &name, GLboolean value) const {
        glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), static_cast<int>(value));
    }

    void set_int(const std::string &name, GLint value) const {
        glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), value);
    }

    void set_float(const std::string &name, GLfloat value) const {
        glUniform1f(glGetUniformLocation(m_program_id, name.c_str()), value);
    }

    void set_uint(const std::string &name, GLuint value) const {
        glUniform1ui(glGetUniformLocation(m_program_id, name.c_str()), value);
    }

    void set_vec(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
        glUniform4f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z, w);
    }

    void set_vec(const std::string &name, GLfloat x, GLfloat y, GLfloat z) const {
        glUniform3f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z);
    }

    void set_vec(const std::string &name, GLfloat x, GLfloat y) const {
        glUniform2f(glGetUniformLocation(m_program_id, name.c_str()), x, y);
    }

    void set_vec(const std::string &name, glm::vec4 value) const {
        glUniform4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_vec(const std::string &name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_vec(const std::string &name, glm::vec2 value) const {
        glUniform2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void set_mat(const std::string &name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat(const std::string &name, glm::mat3 value) const {
        glUniformMatrix3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat(const std::string &name, glm::mat2 value) const {
        glUniformMatrix2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    GLuint load_shader(std::string source_path, GLenum shader_type) const;

private:
    GLuint m_program_id;
};

}

