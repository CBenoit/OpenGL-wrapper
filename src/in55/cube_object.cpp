#include "cube_object.hpp"

#include <vector>
#include <numeric>

#include <glm/glm.hpp>

#include <ow/vertex.hpp>

using namespace std;
using namespace glm;

inline std::vector<ow::vertex> generate_vertices() {
	std::vector<ow::vertex> vertices;

    vertices.emplace_back(vec3(-1.0, -1.0, -1.0), vec3());
    vertices.emplace_back(vec3(-1.0, -1.0,  1.0), vec3());
    vertices.emplace_back(vec3( 1.0, -1.0, -1.0), vec3());
    vertices.emplace_back(vec3( 1.0, -1.0,  1.0), vec3());
    vertices.emplace_back(vec3(-1.0,  1.0, -1.0), vec3());
    vertices.emplace_back(vec3(-1.0,  1.0,  1.0), vec3());
    vertices.emplace_back(vec3( 1.0,  1.0, -1.0), vec3());
    vertices.emplace_back(vec3( 1.0,  1.0,  1.0), vec3());

	return std::move(vertices);
}

inline std::vector<unsigned int> generate_indices() {
	std::vector<unsigned int> indices;

    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(1);
    indices.emplace_back(3);
    indices.emplace_back(2);

    indices.emplace_back(4);
    indices.emplace_back(5);
    indices.emplace_back(6);
    indices.emplace_back(5);
    indices.emplace_back(7);
    indices.emplace_back(6);

    indices.emplace_back(0);
    indices.emplace_back(4);
    indices.emplace_back(5);
    indices.emplace_back(4);
    indices.emplace_back(0);
    indices.emplace_back(1);

    indices.emplace_back(2);
    indices.emplace_back(3);
    indices.emplace_back(6);
    indices.emplace_back(3);
    indices.emplace_back(7);
    indices.emplace_back(6);

    indices.emplace_back(0);
    indices.emplace_back(4);
    indices.emplace_back(2);
    indices.emplace_back(4);
    indices.emplace_back(6);
    indices.emplace_back(2);

    indices.emplace_back(1);
    indices.emplace_back(5);
    indices.emplace_back(3);
    indices.emplace_back(5);
    indices.emplace_back(7);
    indices.emplace_back(3);

	return std::move(indices);
}

cube_object::cube_object() noexcept
	: mesh(generate_vertices(), generate_indices()) {}
