#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ow {

const std::string TEXTURE_DIFFUSE = "diffuse";
const std::string TEXTURE_SPECULAR = "specular";
const std::string TEXTURE_EMISSION = "emission";

struct texture {
    texture(unsigned int id_, std::string type_ = "") : id(id_), type(type_) {}
    texture(std::string filename, std::string type_ = "");
    texture(const texture& other) = delete;
    texture(texture&& other);
    ~texture();
    texture& operator=(const texture& other) = delete;

    GLuint id;
    std::string type;
};

}

