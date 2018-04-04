#include <string>
#include <iostream>
#include <utility>

#include <stb_image.h>

#include <ow/texture.hpp>
#include <ow/utils.hpp>
#include <ow/opengl_codes.hpp>

ow::texture::texture(const std::string& filename, texture_type type_) : id{}, type(type_) {
    // load and generate the texture
    int width, height, nbr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nbr_channels, 0);
    if (data) {
        GLenum format;
        switch (nbr_channels) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            abort();
        }

        auto gl_chk = [] () {check_errors("Error configuring texture " + std::to_string(id));};

        glGenTextures(1, &id);
        check_errors("Error while generating texture.");
        glBindTexture(GL_TEXTURE_2D, id);
	    gl_chk();
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	    gl_chk();
        glGenerateMipmap(GL_TEXTURE_2D);
	    gl_chk();

        // set the texture wrapping/filtering options (on the currently bound
        // texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    gl_chk();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    gl_chk();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	    gl_chk();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    gl_chk();

        glBindTexture(GL_TEXTURE_2D, 0);
	    gl_chk();
    } else {
        logger << "Failed to load texture " << filename << '\n';
    }

    stbi_image_free(data);
}

ow::texture::texture(texture&& other) noexcept
        : id{std::exchange(other.id, 0)}
        , type{other.type}
        {}

ow::texture::~texture() {
    glDeleteTextures(1, &id);
}

std::string ow::texture::type_to_string() const {
    switch (type) {
    case texture_type::diffuse:
        return "diffuse";
    case texture_type::specular:
        return "specular";
    case texture_type::emission:
        return "emission";
    default:
        return "";
    }
}
