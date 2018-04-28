#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <ow/shader_program.hpp>
#include <ow/texture.hpp>
#include <ow/vertex.hpp>
#include <ow/VBO.hpp>

namespace ow {

class mesh {
public:
	mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices,
	     std::vector<std::shared_ptr<texture>> diffuse_maps,
	     std::vector<std::shared_ptr<texture>> specular_maps,
	     std::vector<std::shared_ptr<texture>> emission_maps
	);
    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices,
         std::vector<std::shared_ptr<texture>> textures);
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

    std::vector<std::shared_ptr<texture>>& get_diffuse_maps() { return m_diffuse_maps; }
    const std::vector<std::shared_ptr<texture>>& get_diffuse_maps() const { return m_diffuse_maps; }

    std::vector<std::shared_ptr<texture>>& get_specular_maps() { return m_specular_maps; }
    const std::vector<std::shared_ptr<texture>>& get_specular_maps() const { return m_specular_maps; }

    std::vector<std::shared_ptr<texture>>& get_emission_maps() { return m_emission_maps; }
    const std::vector<std::shared_ptr<texture>>& get_emission_maps() const { return m_emission_maps; }

    void add_texture(std::shared_ptr<texture> texture);

private:
    void _setup_mesh();

	void _activate_next_texture_unit(const shader_program& prog, int* next_unit_to_activate,
	                                 unsigned int current_pass, std::vector<std::shared_ptr<texture>> textures,
	                                 ow::texture_type tex_type) const;

private:
    // render data
    unsigned int m_VAO, m_EBO;
    VBO<1> m_VBO;

    // mesh data
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<std::shared_ptr<texture>> m_diffuse_maps;
    std::vector<std::shared_ptr<texture>> m_specular_maps;
    std::vector<std::shared_ptr<texture>> m_emission_maps;
};

}
