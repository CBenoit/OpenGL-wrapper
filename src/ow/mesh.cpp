#include <cstddef> // offsetof

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <ow/shader_program.hpp>
#include <ow/texture.hpp>
#include <ow/vertex.hpp>
#include <ow/mesh.hpp>

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices,
               std::vector<std::shared_ptr<texture>> textures)
		: m_VAO{}
		, m_EBO{}
		, m_VBO{}
		, m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_textures(std::move(textures)) {
	setup_mesh();
}

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices)
		: m_VAO{}
		, m_EBO{}
		, m_VBO{}
		, m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_textures() {
	setup_mesh();
}

ow::mesh::mesh(mesh &&other) noexcept(noexcept(std::vector<vertex>{std::vector<vertex>{}}))
		: m_VAO{std::exchange(other.m_VAO, 0)}
		, m_EBO{std::exchange(other.m_EBO, 0)}
		, m_VBO{std::move(other.m_VBO)}
		, m_vertices{std::move(other.m_vertices)}
		, m_indices{std::move(other.m_indices)}
		, m_textures{std::move(other.m_textures)}
{}

ow::mesh::~mesh() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_EBO);
}

void ow::mesh::setup_mesh() {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	m_VBO.set_data(m_vertices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// vertex positions
	m_VBO.attribs().layout_size = 3u;
	m_VBO.attribs().stride = 1;
	m_VBO.flush_layout_attribs();

	// vertex normals
	m_VBO.attribs().layout_index = 1u;
	m_VBO.attribs().layout_offset = offsetof(vertex, normal);
	m_VBO.flush_layout_attribs();

	// vertex texture coords
	m_VBO.attribs().layout_index = 2u;
	m_VBO.attribs().layout_size = 2u;
	m_VBO.attribs().layout_offset = offsetof(vertex, tex_coords);
	m_VBO.flush_layout_attribs();

	glBindVertexArray(0); // unbind the VAO
}

void ow::mesh::draw(const shader_program &prog) const {
	prog.use();

	unsigned int nbr_diffuse = 0;
	unsigned int nbr_specular = 0;
	unsigned int nbr_emission = 0;
	for (unsigned int i = 0; i < m_textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding

		// retrieve texture number
		std::string number;
		std::string type = m_textures[i]->type;
		if (type == ow::TEXTURE_DIFFUSE) {
			number = std::to_string(nbr_diffuse++);

		} else if (type == ow::TEXTURE_SPECULAR) {
			number = std::to_string(nbr_specular++);

		} else if (type == ow::TEXTURE_EMISSION) {
			number = std::to_string(nbr_emission++);

		}

		prog.set((type + "_maps[" + number + "]").c_str(), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
	}
	prog.set("nbr_diffuse_maps", nbr_diffuse);
	prog.set("nbr_specular_maps", nbr_specular);
	prog.set("nbr_emission_maps", nbr_emission);
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

