#include <cstddef> // offsetof

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <ow/shader_program.hpp>
#include <ow/texture.hpp>
#include <ow/vertex.hpp>
#include <ow/mesh.hpp>

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<texture>> textures)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_textures(textures) {
    setup_mesh();
}

ow::mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_textures() {
    setup_mesh();
}

ow::mesh::mesh(mesh&& other) {
    m_VAO = other.m_VAO;
    m_VBO = other.m_VBO;
    m_EBO = other.m_EBO;
    m_vertices = other.m_vertices;
    m_indices = other.m_indices;
    m_textures = other.m_textures;

    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

ow::mesh::~mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void ow::mesh::setup_mesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) 0);
    glEnableVertexAttribArray(0);
    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, normal));
    glEnableVertexAttribArray(1);
    // vertex texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, tex_coords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // unbind the VAO
}

void ow::mesh::draw(shader_program prog) const {
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

        prog.set_int((type + "_maps[" + number + "]").c_str(), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
    }
    prog.set_int("nbr_diffuse_maps", nbr_diffuse);
    prog.set_int("nbr_specular_maps", nbr_specular);
    prog.set_int("nbr_emission_maps", nbr_emission);
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

