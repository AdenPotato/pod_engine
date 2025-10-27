#include "sprite.h"
#include "texture.h"
#include <iostream>

Sprite::Sprite()
    : m_texture(nullptr)
    , m_size(1.0f, 1.0f)
    , m_offset(0.0f, 0.0f)
    , m_loaded(false)
{
}

Sprite::~Sprite() {
    if (m_texture) {
        delete m_texture;
        m_texture = nullptr;
    }
}

bool Sprite::loadFromFile(const std::string& filepath) {
    m_texture = new Texture();

    if (!m_texture->loadFromFile(filepath, true)) {
        std::cerr << "Failed to load sprite: " << filepath << std::endl;
        delete m_texture;
        m_texture = nullptr;
        return false;
    }

    // Calculate size in meters
    // GZDoom-style: Use pixel dimensions but scale to reasonable size
    // Assume 64 pixels = 1 meter (common Doom scale)
    float pixelsPerMeter = 64.0f;
    m_size.x = m_texture->getWidth() / pixelsPerMeter;
    m_size.y = m_texture->getHeight() / pixelsPerMeter;

    // Default offset is centered at bottom (like Doom sprites)
    m_offset = glm::vec2(0.0f, 0.0f);

    m_loaded = true;

    std::cout << "Sprite loaded: " << filepath
              << " (size: " << m_size.x << "x" << m_size.y << "m)" << std::endl;

    return true;
}
