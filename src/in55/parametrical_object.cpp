#include "parametrical_object.hpp"

#include <vector>
#include <numeric>

#include <glm/glm.hpp>

#include <ow/vertex.hpp>

using namespace std;
using namespace glm;

inline vec3 compute_normal(vec3 v1, vec3 v2, vec3 v3) {
	return normalize(cross(v2-v1, v3-v1));
}

inline std::vector<ow::vertex> generate_vertices(unsigned int n) {
	std::vector<ow::vertex> vertices;
	double step = 2*M_PI / n;

	// Layer2
	for (unsigned int i = 0; i < n; i++) {
		double angle = static_cast<double>(i) * step;

		{ // top
			vec3 v1{0.0, 1.0, 0.0};
			vec3 v2{0.5 * sin(angle), 1.0, 0.5 * cos(angle)};
			vec3 v3{0.5 * sin(angle + step), 1.0, 0.5 * cos(angle + step)};
			vec3 norm = compute_normal(v1, v2, v3);
			vertices.emplace_back(v1, norm);
			vertices.emplace_back(v2, norm);
			vertices.emplace_back(v3, norm);
		}

		{ // first angles
			vec3 v1{0.5 * sin(angle), 1.0, 0.5 * cos(angle)};
			vec3 v2{0.75 * sin(angle + step / 2.0), 0.75, 0.75 * cos(angle + step / 2.0)};
			vec3 v3{0.5 * sin(angle + step), 1.0, 0.5 * cos(angle + step)};
			vec3 norm = compute_normal(v1, v2, v3);
			vertices.emplace_back(v1, norm);
			vertices.emplace_back(v2, norm);
			vertices.emplace_back(v3, norm);
		}

		{ // second angles
			vec3 v1{0.5 * sin(angle), 1, 0.5 * cos(angle)};
			vec3 v2{0.75 * sin(angle - step / 2.0), 0.75, 0.75 * cos(angle - step / 2.0)};
			vec3 v3{0.75 * sin(angle + step / 2.0), 0.75, 0.75 * cos(angle + step / 2.0)};
			vec3 norm = compute_normal(v1, v2, v3);
			vertices.emplace_back(v1, norm);
			vertices.emplace_back(v2, norm);
			vertices.emplace_back(v3, norm);
		}

		{ // bottom angles
			vec3 v1{0.75 * sin(angle - step / 2.0), 0.75, 0.75 * cos(angle - step / 2.0)};
			vec3 v2{0.0, 0.0, 0.0};
			vec3 v3{0.75 * sin(angle + step / 2.0), 0.75, 0.75 * cos(angle + step / 2.0)};
			vec3 norm = compute_normal(v1, v2, v3);
			vertices.emplace_back(v1, norm);
			vertices.emplace_back(v2, norm);
			vertices.emplace_back(v3, norm);
		}
	}

	return std::move(vertices);
}

inline std::vector<unsigned int> generate_indices(unsigned int n) {
	std::vector<unsigned int> indices(n * 3 * 4);
	std::iota(indices.begin(), indices.end(), 0);
	return std::move(indices);
}

parametrical_object::parametrical_object(unsigned int n) noexcept
	: mesh(generate_vertices(n), generate_indices(n)) {}
