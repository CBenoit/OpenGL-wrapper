#pragma once

#include <glm/glm.hpp>

#include <ow/base_light.hpp>

namespace ow {

class spotlight : public base_light {
public:
    spotlight(glm::vec3 position, glm::vec3 direction, float cutoff, float outer_cutoff,
                float attenuation_constant, float attenuation_linear, float attenuation_quadratic)
        : base_light()
        , m_position(position)
        , m_direction(direction)
        , m_cutoff(cutoff)
        , m_outer_cutoff(outer_cutoff)
        , m_attenuation_constant(attenuation_constant)
        , m_attenuation_linear(attenuation_linear)
        , m_attenuation_quadratic(attenuation_quadratic) {}

    spotlight(glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position,
                glm::vec3 direction, float cutoff, float outer_cutoff,
                float attenuation_constant, float attenuation_linear, float attenuation_quadratic)
        : base_light(diffuse, specular)
        , m_position(position)
        , m_direction(direction)
        , m_cutoff(cutoff)
        , m_outer_cutoff(outer_cutoff)
        , m_attenuation_constant(attenuation_constant)
        , m_attenuation_linear(attenuation_linear)
        , m_attenuation_quadratic(attenuation_quadratic) {}

    void update_all(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "spotlight.") const override {
        base_light::update_all(prog, view, uniform_prefix);

        prog.set_vec(uniform_prefix + "position", glm::vec3(view * glm::vec4(m_position, 1)));
        prog.set_vec(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));

        prog.set_float(uniform_prefix + "cutoff", m_cutoff);
        prog.set_float(uniform_prefix + "outer_cutoff", m_outer_cutoff);

        prog.set_float(uniform_prefix + "attenuation_constant", m_attenuation_constant);
        prog.set_float(uniform_prefix + "attenuation_linear", m_attenuation_linear);
        prog.set_float(uniform_prefix + "attenuation_quadratic", m_attenuation_quadratic);
    }

    void update_position_and_direction(const shader_program& prog, glm::mat4 view, std::string uniform_prefix = "spotlight.") const {
        prog.set_vec(uniform_prefix + "position", glm::vec3(view * glm::vec4(m_position, 1)));
        prog.set_vec(uniform_prefix + "direction", glm::vec3(view * glm::vec4(m_direction, 0)));
    }

    void set_pos(glm::vec3 pos) {
        m_position = pos;
    }

    void set_dir(glm::vec3 dir) {
        m_direction = dir;
    }

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;

    float m_cutoff;
    float m_outer_cutoff;

    float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;
};

}


