#pragma once

#include <glm/glm.hpp>

#include <ow/base_light.hpp>

namespace ow {

class directional_light : public base_light {
public:
    directional_light(glm::vec3 direction)
        : base_light()
        , m_direction(direction) {}

    directional_light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction)
        : base_light(ambient, diffuse, specular)
        , m_direction(direction) {}

    void update_all(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "dir_light.") const override {
        base_light::update_all(prog, view, uniform_prefix);

        prog.set(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));
    }

    void update_direction(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "dir_light.") const {
        prog.set(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));
    }

    void set_dir(glm::vec3 dir) {
        m_direction = dir;
    }

private:
    glm::vec3 m_direction;
};

}

