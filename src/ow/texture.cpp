#include <string>
#include <iostream>

#include <stb_image.h>

#include <ow/texture.hpp>

ow::texture::texture(std::string filename, std::string type_) : type(type_) {
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
        }

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping/filtering options (on the currently bound
        // texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        std::cout << "Failed to load texture " << filename << std::endl;
    }

    stbi_image_free(data);
}

ow::texture::texture(texture&& other) {
    id = other.id;
    other.id = 0;
}

ow::texture::~texture() {
    glDeleteTextures(1, &id);
}

