#pragma once

#include <ow/VBO.hpp>
#include <ow/shader_program.hpp>

class lamp {
public:
	lamp() noexcept;
	lamp(const lamp& other) = delete;
	lamp(lamp&& other) noexcept;
	~lamp();
	lamp& operator=(const lamp& other) = delete;

	void draw(const ow::shader_program& prog) const;

private:
	// render data
	GLuint m_VAO, m_EBO;
	ow::VBO<1> m_VBO;
	unsigned long m_indices_size;
};
