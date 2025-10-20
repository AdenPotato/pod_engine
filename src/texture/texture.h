#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    // Prevent copying
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Allow moving
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // Load texture from file
    bool loadFromFile(const std::string& path, bool generateMipmaps = true);

    // Bind texture to a texture unit
    void bind(unsigned int unit = 0) const;

    // Unbind texture
    void unbind() const;

    // Getters
    unsigned int getID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getChannels() const { return m_channels; }
    bool isLoaded() const { return m_loaded; }

    // Texture parameters
    void setWrapMode(GLenum wrapS, GLenum wrapT);
    void setFilterMode(GLenum minFilter, GLenum magFilter);

private:
    void cleanup();

    unsigned int m_textureID;
    int m_width;
    int m_height;
    int m_channels;
    bool m_loaded;
};
