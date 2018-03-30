#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ow/shader_program.hpp>
#include <ow/utils.hpp>

ow::shader_program::shader_program(shader_program&& other) noexcept
        : checkable(other.p_state)
        , m_program_id{std::exchange(other.m_program_id, 0)}
{}

bool ow::shader_program::put(const std::vector<std::pair<GLenum, std::string_view>>& shaders) {
    chk_state();

    for (auto&& shader : shaders) {
        auto shader_id = glCreateShader(shader.first);
        if (!load_compile(shader_id, shader.second)) {
            p_state = false;
            return false;
        }

        glAttachShader(get_id(), shader_id);
        p_state = check_errors(
                "Error while attaching " + std::to_string(shader_id) + " to shader " + std::to_string(get_id()) + "\n");
        if (!p_state) {
            return false;
        }

        glDeleteShader(shader_id);
        p_state = check_errors("Error while flagging " + std::to_string(shader_id)
                               + " for deletion (in shader " + std::to_string(get_id()) + "\n");
        if (!p_state) {
            return false;
        }
    }
    glLinkProgram(get_id());
    p_state = check_errors("Unexpected error while linking shader " + std::to_string(get_id()) + ".\n");
    return static_cast<bool>(*this);
}



bool ow::shader_program::load_shader(GLuint shader, std::string_view file_name) {
    using namespace std::string_literals;

    std::ifstream file("resources/shaders/"s + file_name.data(), std::ios::in);
    if (!file) {
        logger << "[" << file_name << "] : failed to open file." << std::endl;
        return false;
    }

    std::string line;
    std::string str_out;
    while (file) {
        std::getline(file, line);
        str_out += line + '\n';
    }

    const char *const tmp = str_out.data();
    glShaderSource(shader, 1, &tmp, nullptr);
    return check_errors("["s + file_name.data() + "] : Error while sourcing code.\n");
}

bool ow::shader_program::checked_compile(GLuint shader, std::string_view shader_file) {
    const std::string failed_compile_status =
            "\tError while retrieving compile status for shader " + std::to_string(shader) + ". ";
    
    glCompileShader(shader);
    check_errors("Error while compiling shader " + std::to_string(shader) + ". ");

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    check_errors(failed_compile_status);

    if (!success || glGetError() != GL_NO_ERROR) {
	    auto&& ostream = logger << "[" << std::string(shader_file) << "] : Failed to compile :\n";

        GLsizei written  = 0;
        char log[256] = "\0";
        glGetShaderInfoLog(shader, sizeof(log) / sizeof(char), &written, log);
        if (!check_errors(failed_compile_status)) {
            return false;
        }

        if (static_cast<unsigned>(written) < sizeof(log) / sizeof(char) - 1) {
	        ostream << log;
        } else {
            std::vector<char> vlog;
            vlog.reserve(2 * sizeof(log) / sizeof(char));
            glGetShaderInfoLog(shader, static_cast<GLsizei>(vlog.capacity()), &written, vlog.data());
            if (!check_errors(failed_compile_status)) {
                return false;
            }

            while (vlog.capacity() == static_cast<unsigned>(written + 1)) {
                vlog.reserve(2 * vlog.capacity());
                glGetShaderInfoLog(shader, static_cast<GLsizei>(vlog.capacity()), &written, vlog.data());
                if (!check_errors(failed_compile_status)) {
                    return false;
                }
            }
	        ostream << vlog.data();

        }

        return false;
    }
    return true;
}

bool ow::shader_program::load_compile(GLuint shader, std::string_view file) {
    return load_shader(shader, file) && checked_compile(shader, file);
}
