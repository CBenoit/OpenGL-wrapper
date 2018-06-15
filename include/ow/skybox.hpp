#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ow {

struct skybox {
	explicit skybox(const std::string& filename);
	skybox(const skybox& other) = delete;
	skybox(skybox&& other) noexcept;
	~skybox();
	skybox& operator=(const skybox& other) = delete;

	GLuint id;
};


}

