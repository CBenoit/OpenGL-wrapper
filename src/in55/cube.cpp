#include "cube.hpp"

#include <vector>

cube::cube() noexcept
	: m_VAO(0)
	, m_EBO(0)
	, m_VBO()
	, m_indices_size(0)
{
	std::array vertices = {
			// front
			-0.5f, -0.5f, 0.5f,
			-0.5f,  0.5f, 0.5f,
			0.5f,  -0.5f, 0.5f,
			0.5f,   0.5f, 0.5f,

			// back
			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			0.5f,  -0.5f, -0.5f,
			0.5f,   0.5f, -0.5f,

			// left
			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			// right
			0.5f, -0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,

			// top
			-0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f,  0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f,  0.5f,

			// bottom
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
	};

	std::vector<GLuint> indices = {
			// front face
			0, 1, 2, // first triangle
			1, 2, 3,  // second triangle
			// back face
			4, 5, 6,
			5, 6, 7,
			// left face
			8, 9, 10,
			9, 10, 11,
			// right face
			12, 13, 14,
			13, 14, 15,
			// top face
			16, 17, 18,
			17, 18, 19,
			// bottom face
			20, 21, 22,
			21, 22, 23,
	};
	m_indices_size = indices.size();

	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	ow::check_errors("Failed to bind EBO. ");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	ow::check_errors("Failed to set EBO data. ");

	m_VBO.set_data(vertices);
	m_VBO.attribs().layout_size = 3u;
	m_VBO.attribs().stride = 0;
	m_VBO.flush_layout_attribs();

	glBindVertexArray(0);
}

cube::cube(cube&& other) noexcept
		: m_VAO{std::exchange(other.m_VAO, 0)}
		, m_EBO{std::exchange(other.m_EBO, 0)}
		, m_VBO{std::move(other.m_VBO)}
		, m_indices_size(other.m_indices_size){}

cube::~cube() {
	glDeleteVertexArrays(1, &m_VAO);
	ow::check_errors("error while deleting VAO. ");
	glDeleteBuffers(1, &m_EBO);
	ow::check_errors("error while deleting EBO. ");
}

void cube::draw(const ow::shader_program& prog) const {
	prog.use();

	glBindVertexArray(m_VAO);
	ow::check_errors("failed to bind VAO. ");

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices_size), GL_UNSIGNED_INT, 0);
	ow::check_errors("failed to draw VAO elements. ");

	glBindVertexArray(0);
	ow::check_errors("failed to unbind VAO. ");
}
