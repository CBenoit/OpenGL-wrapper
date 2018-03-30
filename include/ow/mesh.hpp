#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <ow/shader_program.hpp>
#include <ow/texture.hpp>
#include <ow/vertex.hpp>

namespace ow {

class mesh {
public:
    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<texture>> textures);
    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices);
    mesh(const mesh& other) = delete;
    mesh(mesh&& other) noexcept(noexcept(std::vector<vertex>{std::move(std::vector<vertex>{})}));
    ~mesh();
    mesh& operator=(const mesh& other) = delete;

    void draw(const shader_program& prog) const;

    std::vector<vertex>& get_vertices() { return m_vertices; }
    const std::vector<vertex>& get_vertices() const { return m_vertices; }

    std::vector<unsigned int>& get_indices() { return m_indices; }
    const std::vector<unsigned int>& get_indices() const { return m_indices; }

    std::vector<std::shared_ptr<texture>>& get_textures() { return m_textures; }
    const std::vector<std::shared_ptr<texture>>& get_textures() const { return m_textures; }

    void add_texture(std::shared_ptr<texture> texture) { m_textures.push_back(std::move(texture)); }

private:
    void setup_mesh();

private:
    // render data
    unsigned int m_VAO, m_VBO, m_EBO;

    // mesh data
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<std::shared_ptr<texture>> m_textures;
};

}

