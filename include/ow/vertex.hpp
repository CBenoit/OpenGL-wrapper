#pragma once

#include <glm/glm.hpp>

namespace ow {

struct vertex {
    vertex(glm::vec3 pos) : position(pos), normal(0), tex_coords(0) {}
    vertex(glm::vec3 pos, glm::vec3 norm) : position(pos), normal(norm), tex_coords(0) {}
    vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texture_coords)
        : position(pos), normal(norm), tex_coords(texture_coords) {}

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

}

