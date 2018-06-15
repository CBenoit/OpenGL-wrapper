#pragma once

#include <ow/VBO.hpp>
#include <ow/shader_program.hpp>

class cube {
public:
	cube() noexcept;
	cube(const cube& other) = delete;
	cube(cube&& other) noexcept;
	~cube();
	cube& operator=(const cube& other) = delete;

	void draw(const ow::shader_program& prog) const;

private:
	// render data
	GLuint m_VAO, m_EBO;
	ow::VBO<1> m_VBO;
	unsigned long m_indices_size;
};
