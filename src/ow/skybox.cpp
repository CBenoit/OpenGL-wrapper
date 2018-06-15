#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include <stb_image.h>

#include <ow/skybox.hpp>
#include <ow/utils.hpp>
#include <ow/opengl_codes.hpp>

ow::skybox::skybox(const std::string& dirname) : id{} {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    std::vector<std::string> filenames = {
        "/right.jpg",
        "/left.jpg",
        "/bottom.jpg",
        "/top.jpg",
        "/front.jpg",
        "/back.jpg"
    };

	// load and generate the skybox
	int width, height, nbr_channels;
	stbi_set_flip_vertically_on_load(true);
    for(GLuint i = 0; i < filenames.size(); i++) {
        unsigned char* data = stbi_load((dirname + filenames[i]).c_str(), &width, &height, &nbr_channels, 0);
        if(data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        } else {
            logger << "Failed to load cubemap texture " << dirname << filenames[i] << '\n';
        }
	    stbi_image_free(data);
    }

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

ow::skybox::skybox(skybox&& other) noexcept
	: id{std::exchange(other.id, 0)} {}

ow::skybox::~skybox() {
	glDeleteTextures(1, &id);
}
