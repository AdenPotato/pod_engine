#include "billboard_renderer.h"
#include "sprite.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

BillboardRenderer::BillboardRenderer()
    : m_shader(nullptr)
    , m_vao(0)
    , m_vbo(0)
    , m_initialized(false)
{
}

BillboardRenderer::~BillboardRenderer() {
    cleanup();
}

void BillboardRenderer::init() {
    if (m_initialized) return;

    // Load billboard shader
    m_shader = new Shader("shaders/billboard.vert", "shaders/billboard.frag");

    // Initialize quad
    initQuad();

    m_initialized = true;
    std::cout << "BillboardRenderer initialized" << std::endl;
}

void BillboardRenderer::initQuad() {
    // Billboard quad vertices (centered at origin)
    // Position (xy) and TexCoords (zw)
    float vertices[] = {
        // pos (centered)     // tex coords
        -0.5f, -0.5f,         0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f,         1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f,         1.0f, 1.0f,  // Top-right

        -0.5f, -0.5f,         0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f,         1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f,         0.0f, 1.0f   // Top-left
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // Texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void BillboardRenderer::drawSprite(
    const Sprite* sprite,
    const glm::vec3& position,
    const Camera& camera,
    float aspectRatio,
    BillboardMode mode,
    const glm::vec3& scale,
    const glm::vec4& color)
{
    if (!m_initialized || !sprite || !sprite->isLoaded()) {
        return;
    }

    m_shader->use();

    // Set matrices
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
    glm::mat4 view = camera.getViewMatrix();

    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);

    // Set sprite world position
    m_shader->setVec3("spritePos", position);

    // Set sprite size (scaled by sprite's actual size)
    glm::vec2 spriteSize = sprite->getSize();
    glm::vec2 finalSize = glm::vec2(spriteSize.x * scale.x, spriteSize.y * scale.y);
    m_shader->setVec2("spriteSize", finalSize);

    // Set color tint
    m_shader->setVec4("spriteColor", color);

    // Calculate right and up vectors based on billboard mode
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;

    if (mode == BillboardMode::YAxisLocked) {
        // Doom-style: Lock to Y axis (no tilting with camera pitch)
        // Right vector is perpendicular to camera forward on XZ plane
        glm::vec3 forward = camera.Front;
        forward.y = 0.0f;  // Project to XZ plane

        if (glm::length(forward) > 0.001f) {
            forward = glm::normalize(forward);
        } else {
            // Camera looking straight up/down, use camera right
            forward = glm::vec3(0.0f, 0.0f, -1.0f);
        }

        cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // Always world up
    } else {
        // Full billboard: Use camera's right and up vectors directly
        cameraRight = camera.Right;
        cameraUp = camera.Up;
    }

    m_shader->setVec3("cameraRight", cameraRight);
    m_shader->setVec3("cameraUp", cameraUp);

    // Bind sprite texture
    sprite->getTexture()->bind(0);
    m_shader->setInt("spriteTexture", 0);

    // Enable alpha blending for sprites
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable face culling for billboards
    glDisable(GL_CULL_FACE);

    // Draw quad
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Restore state
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void BillboardRenderer::beginBatch() {
    // TODO: Implement batch rendering for performance
}

void BillboardRenderer::endBatch() {
    // TODO: Implement batch rendering for performance
}

void BillboardRenderer::cleanup() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_shader) {
        delete m_shader;
        m_shader = nullptr;
    }
    m_initialized = false;
}
