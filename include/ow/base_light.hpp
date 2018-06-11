#pragma once

#include <glm/glm.hpp>

#include <ow/shader_program.hpp>

namespace ow {

class base_light {
public:
	explicit base_light(glm::vec3 diffuse = glm::vec3(0.5f), glm::vec3 specular = glm::vec3(1.0f))
		: m_diffuse(diffuse)
		, m_specular(specular) {}

	virtual ~base_light() noexcept = default;

	virtual void update_all(const shader_program& prog, glm::mat4 /*view*/, const std::string& uniform_prefix = "light.") const {
		prog.set(uniform_prefix + "diffuse", m_diffuse);
		prog.set(uniform_prefix + "specular", m_specular);
	}

	void set_diffuse(glm::vec3 diffuse) {
		m_diffuse = diffuse;
	}

	void set_specular(glm::vec3 specular) {
		m_specular = specular;
	}

private:
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
};

}

