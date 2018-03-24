#pragma once

#include <glm/glm.hpp>

#include <ow/shader_program.hpp>

namespace ow {

class base_light {
public:
    base_light(glm::vec3 ambient = glm::vec3(0.2f), glm::vec3 diffuse = glm::vec3(0.5f), glm::vec3 specular = glm::vec3(1.0f))
        : m_ambient(ambient)
        , m_diffuse(diffuse)
        , m_specular(specular) {}

    virtual void update_all(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "light.") const {
        prog.set(uniform_prefix + "ambient", m_ambient);
        prog.set(uniform_prefix + "diffuse", m_diffuse);
        prog.set(uniform_prefix + "specular", m_specular);
    }

private:
    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
};

}

