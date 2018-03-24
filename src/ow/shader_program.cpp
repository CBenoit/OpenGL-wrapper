#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ow/shader_program.hpp>

ow::shader_program::shader_program(std::string vertex_file_path, std::string fragment_file_path) {
    GLuint vertex_shader = load_shader(vertex_file_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = load_shader(fragment_file_path, GL_FRAGMENT_SHADER);

    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, vertex_shader);
    glAttachShader(m_program_id, fragment_shader);
    glLinkProgram(m_program_id);

    int success;
    glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_program_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
    }

    glDetachShader(m_program_id, vertex_shader);
    glDetachShader(m_program_id, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

GLuint ow::shader_program::load_shader(std::string source_path, GLenum shader_type) const {
    std::string shader_src;
    std::ifstream shader_file;
    // ensure ifstream can throw exceptions
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shader_file.open(source_path);
        std::stringstream shader_src_stream;
        shader_src_stream << shader_file.rdbuf();
        shader_file.close();
        shader_src = shader_src_stream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    GLuint shader_id;
    shader_id = glCreateShader(shader_type);

    // compile shader
    std::cout << "Compiling shader: " << source_path << std::endl;
    const char* shader_src_pointer = shader_src.c_str();
    glShaderSource(shader_id, 1, &shader_src_pointer, NULL); // attach source
    glCompileShader(shader_id); // compile

    // error checking
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED (" << source_path << ")\n"
                    << infoLog << std::endl;
    }

    return shader_id;
}

