#pragma once

#include <glm/glm.hpp>
#include <string>

class Texture;

// Represents a single sprite frame (like Doom's TROOA0, PISGA0, etc.)
class Sprite {
public:
    Sprite();
    ~Sprite();

    // Load sprite from file
    bool loadFromFile(const std::string& filepath);

    // Getters
    Texture* getTexture() const { return m_texture; }
    glm::vec2 getSize() const { return m_size; }
    glm::vec2 getOffset() const { return m_offset; }
    bool isLoaded() const { return m_loaded; }

    // Setters (for custom offset/pivot)
    void setOffset(const glm::vec2& offset) { m_offset = offset; }

private:
    Texture* m_texture;
    glm::vec2 m_size;      // Size in meters (derived from pixel size)
    glm::vec2 m_offset;    // Drawing offset from center
    bool m_loaded;
};
