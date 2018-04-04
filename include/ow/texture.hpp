#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ow {

enum class texture_type {
	diffuse,
	specular,
	emission
};

struct texture {
    explicit texture(unsigned int id_, texture_type type_ = texture_type::emission) : id(id_), type(type_) {}
    explicit texture(const std::string& filename, texture_type type_ = texture_type::emission);
    texture(const texture& other) = delete;
    texture(texture&& other) noexcept;
    ~texture();
    texture& operator=(const texture& other) = delete;

    std::string type_to_string() const;

    GLuint id;
    texture_type type;
};

}

