#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ow {

constexpr std::string_view TEXTURE_DIFFUSE = "diffuse";
constexpr std::string_view TEXTURE_SPECULAR = "specular";
constexpr std::string_view TEXTURE_EMISSION = "emission";

struct texture {
    explicit texture(unsigned int id_, std::string type_ = "") : id(id_), type(std::move(type_)) {}
    explicit texture(const std::string& filename, std::string_view type_ = "");
    texture(const texture& other) = delete;
    texture(texture&& other) noexcept;
    ~texture();
    texture& operator=(const texture& other) = delete;

    GLuint id;
    std::string type;
};

}

