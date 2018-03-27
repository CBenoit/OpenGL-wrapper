#pragma once

#include <memory>
#include <vector>
#include <sstream>

#include <glm/glm.hpp>

#include <ow/shader_program.hpp>
#include <ow/directional_light.hpp>
#include <ow/point_light.hpp>
#include <ow/spotlight.hpp>

namespace ow {

class lights_set {
public:
    lights_set() : m_dir_lights(), m_point_lights(), m_spotlights() {}

    void update_all(const shader_program& prog, glm::mat4 view) const {
        prog.set_int("nbr_dir_lights", m_dir_lights.size());
        for (unsigned int i = m_dir_lights.size(); i--;) {
            m_dir_lights[i]->update_all(prog, view, "dir_lights[" + std::to_string(i) + "].");
        }

        prog.set_int("nbr_point_lights", m_point_lights.size());
        for (unsigned int i = m_point_lights.size(); i--;) {
            m_point_lights[i]->update_all(prog, view, "point_lights[" + std::to_string(i) + "].");
        }

        prog.set_int("nbr_spotlights", m_spotlights.size());
        for (unsigned int i = m_spotlights.size(); i--;) {
            m_spotlights[i]->update_all(prog, view, "spotlights[" + std::to_string(i) + "].");
        }
    }

    void update_position_and_direction(const shader_program& prog, glm::mat4 view) {
        prog.set_int("nbr_dir_lights", m_dir_lights.size());
        for (unsigned int i = m_dir_lights.size(); i--;) {
            m_dir_lights[i]->update_direction(prog, view, "dir_lights[" + std::to_string(i) + "].");
        }

        prog.set_int("nbr_point_lights", m_point_lights.size());
        for (unsigned int i = m_point_lights.size(); i--;) {
            m_point_lights[i]->update_position(prog, view, "point_lights[" + std::to_string(i) + "].");
        }

        prog.set_int("nbr_spotlights", m_spotlights.size());
        for (unsigned int i = m_spotlights.size(); i--;) {
            m_spotlights[i]->update_position_and_direction(prog, view, "spotlights[" + std::to_string(i) + "].");
        }
    }

    void add_directional_light(std::shared_ptr<directional_light> dir_light_ptr) {
        m_dir_lights.push_back(dir_light_ptr);
    }

    void add_point_light(std::shared_ptr<point_light> point_light_ptr) {
        m_point_lights.push_back(point_light_ptr);
    }

    void add_spotlight(std::shared_ptr<spotlight> spotlight_ptr) {
        m_spotlights.push_back(spotlight_ptr);
    }

private:
    std::vector<std::shared_ptr<directional_light>> m_dir_lights;
    std::vector<std::shared_ptr<point_light>> m_point_lights;
    std::vector<std::shared_ptr<spotlight>> m_spotlights;
};

}

