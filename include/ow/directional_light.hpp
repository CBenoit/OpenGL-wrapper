#pragma once

#include <glm/glm.hpp>

#include <ow/base_light.hpp>

namespace ow {

class directional_light : public base_light {
public:
    directional_light(glm::vec3 direction, glm::vec3 ambient = glm::vec3(0.2f))
        : base_light()
        , m_direction(direction)
        , m_ambient(ambient) {}

    directional_light(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : base_light(diffuse, specular)
        , m_direction(direction)
        , m_ambient(ambient) {}

    void update_all(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "dir_light.") const override {
        base_light::update_all(prog, view, uniform_prefix);

        prog.set_vec(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));
        prog.set_vec(uniform_prefix + "ambient", m_ambient);
    }

    void update_direction(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "dir_light.") const {
        prog.set_vec(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));
    }

    void set_dir(glm::vec3 dir) {
        m_direction = dir;
    }

private:
    glm::vec3 m_direction;
    glm::vec3 m_ambient;
};

}

