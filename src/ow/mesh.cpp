#include <cstddef> // offsetof

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <ow/shader_program.hpp>
#include <ow/texture.hpp>
#include <ow/vertex.hpp>
#include <ow/mesh.hpp>

ow::mesh::mesh(std::vector<ow::vertex> vertices, std::vector<unsigned int> indices,
               std::vector<std::shared_ptr<ow::texture>> diffuse_maps,
               std::vector<std::shared_ptr<ow::texture>> specular_maps,
               std::vector<std::shared_ptr<ow::texture>> emission_maps)
		: m_VAO{}, m_EBO{}, m_VBO{}
		, m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_diffuse_maps(std::move(diffuse_maps))
		, m_specular_maps(std::move(specular_maps))
		, m_emission_maps(std::move(emission_maps)) {}

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<texture>> textures)
		: m_VAO{}, m_EBO{}, m_VBO{}
		, m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_diffuse_maps()
		, m_specular_maps()
		, m_emission_maps() {
	for (auto& tex : textures) {
		add_texture(std::move(tex));
	}
	_setup_mesh();
}

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices)
		: m_VAO{}, m_EBO{}, m_VBO{}
	    , m_vertices(std::move(vertices))
	    , m_indices(std::move(indices))
	    , m_diffuse_maps()
		, m_specular_maps()
		, m_emission_maps() {
	_setup_mesh();
}

ow::mesh::mesh(mesh&& other) noexcept(noexcept(std::vector<vertex>{std::vector<vertex>{}}))
	    : m_VAO{std::exchange(other.m_VAO, 0)}
	    , m_EBO{std::exchange(other.m_EBO, 0)}
		, m_VBO{std::move(other.m_VBO)}
	    , m_vertices{std::move(other.m_vertices)}
	    , m_indices{std::move(other.m_indices)}
		, m_diffuse_maps(std::move(other.m_diffuse_maps))
		, m_specular_maps(std::move(other.m_specular_maps))
		, m_emission_maps(std::move(other.m_emission_maps)) {}

ow::mesh::~mesh() {
	glDeleteVertexArrays(1, &m_VAO);
	check_errors("error while deleting VAO. ");
	glDeleteBuffers(1, &m_EBO);
	check_errors("error while deleting EBO. ");
}

void ow::mesh::draw(const shader_program& prog) const {
    prog.use();

	glBindVertexArray(m_VAO);
	check_errors("failed to bind VAO. ");
    size_t number_of_passes = std::max(std::max(m_diffuse_maps.size(), m_specular_maps.size()), m_emission_maps.size());
    for (unsigned int i = 0; i < number_of_passes; ++i) {
    	int next_unit_to_activate = 0;
	    _activate_next_texture_unit(prog, &next_unit_to_activate, i, m_diffuse_maps, texture_type::diffuse);
	    _activate_next_texture_unit(prog, &next_unit_to_activate, i, m_specular_maps, texture_type::specular);
	    _activate_next_texture_unit(prog, &next_unit_to_activate, i, m_emission_maps, texture_type::emission);

	    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
	    check_errors("failed to draw VAO elements. ");
    }
	// reset
    glActiveTexture(GL_TEXTURE0);
	check_errors("error while activating texture " + std::to_string(GL_TEXTURE0) + ". ");
	glBindVertexArray(0);
	check_errors("failed to unbind VAO. ");
}

void ow::mesh::add_texture(std::shared_ptr<ow::texture> texture) {
	switch (texture->type) {
	case texture_type::diffuse:
		m_diffuse_maps.push_back(texture);
		break;
	case texture_type::specular:
		m_specular_maps.push_back(texture);
		break;
	case texture_type::emission:
		m_emission_maps.push_back(texture);
		break;
	default:
		break; // nothing to do.
	}
}

void ow::mesh::_setup_mesh() {
	glGenVertexArrays(1, &m_VAO);
	check_errors("error while generating VAO. ");
	glGenBuffers(1, &m_EBO);
	check_errors("error while generating EBO. ");

	glBindVertexArray(m_VAO);
	check_errors("Failed to bind VAO. ");

	m_VBO.set_data(m_vertices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	check_errors("Failed to bind EBO. ");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
	check_errors("Failed to set EBO data. ");

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
	check_errors("Failed to unbind VAO. ");
}

void ow::mesh::_activate_next_texture_unit(const shader_program& prog, int* next_unit_to_activate,
                                           unsigned int current_pass,
                                           std::vector<std::shared_ptr<ow::texture>> textures,
                                           ow::texture_type tex_type) const {
	if (current_pass < textures.size()) {
		glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(*next_unit_to_activate));
		check_errors("error while activating texture unit " + std::to_string(GL_TEXTURE0 + (*next_unit_to_activate)) + ". ");
		glBindTexture(GL_TEXTURE_2D, textures[current_pass]->id);
		check_errors("error while binding texture " + std::to_string(textures[current_pass]->id) + ". ");

		prog.set("has_" + textures[current_pass]->type_to_string() + "_map", true);
		prog.set(textures[current_pass]->type_to_string() + "_map", *next_unit_to_activate);

		++(*next_unit_to_activate);
	} else {
		prog.set("has_" + texture_type_to_string(tex_type) + "_map", false);
	}
}
