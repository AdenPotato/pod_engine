#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture()
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_channels(0)
    , m_loaded(false)
{
}

Texture::~Texture() {
    cleanup();
}

Texture::Texture(Texture&& other) noexcept
    : m_textureID(other.m_textureID)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_channels(other.m_channels)
    , m_loaded(other.m_loaded)
{
    other.m_textureID = 0;
    other.m_loaded = false;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        cleanup();

        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_loaded = other.m_loaded;

        other.m_textureID = 0;
        other.m_loaded = false;
    }
    return *this;
}

bool Texture::loadFromFile(const std::string& path, bool generateMipmaps) {
    cleanup();

    // Load image using stb_image
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // Determine format based on number of channels
    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;

    if (m_channels == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    } else if (m_channels == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB;
    } else if (m_channels == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }

    // Generate texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps if requested
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Free image data
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_loaded = true;
    std::cout << "Texture loaded successfully: " << path
              << " (" << m_width << "x" << m_height << ", " << m_channels << " channels)" << std::endl;

    return true;
}

void Texture::bind(unsigned int unit) const {
    if (m_loaded) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapMode(GLenum wrapS, GLenum wrapT) {
    if (m_loaded) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::setFilterMode(GLenum minFilter, GLenum magFilter) {
    if (m_loaded) {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::cleanup() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_loaded = false;
}
