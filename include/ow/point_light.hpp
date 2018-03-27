#pragma once

#include <glm/glm.hpp>

#include <ow/base_light.hpp>

namespace ow {

class point_light : public base_light {
public:
    point_light(glm::vec3 position, float attenuation_constant, float attenuation_linear, float attenuation_quadratic)
        : base_light()
        , m_position(position)
        , m_attenuation_constant(attenuation_constant)
        , m_attenuation_linear(attenuation_linear)
        , m_attenuation_quadratic(attenuation_quadratic) {}

    point_light(glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position,
                float attenuation_constant, float attenuation_linear, float attenuation_quadratic)
        : base_light(diffuse, specular)
        , m_position(position)
        , m_attenuation_constant(attenuation_constant)
        , m_attenuation_linear(attenuation_linear)
        , m_attenuation_quadratic(attenuation_quadratic) {}

    void update_all(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "point_light.") const override {
        base_light::update_all(prog, view, uniform_prefix);

        prog.set_vec(uniform_prefix + "position", glm::vec3(view * glm::vec4(m_position, 1)));

        prog.set_float(uniform_prefix + "attenuation_constant", m_attenuation_constant);
        prog.set_float(uniform_prefix + "attenuation_linear", m_attenuation_linear);
        prog.set_float(uniform_prefix + "attenuation_quadratic", m_attenuation_quadratic);
    }

    void update_position(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "point_light.") const {
        prog.set_vec(uniform_prefix + "position", glm::vec3(view * glm::vec4(m_position, 1)));
    }

    void set_pos(glm::vec3 pos) {
        m_position = pos;
    }

    glm::vec3 get_pos() {
        return m_position;
    }

private:
    glm::vec3 m_position;

    float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;
};

}

