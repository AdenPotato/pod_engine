#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Sprite;
class Shader;
class Camera;

// Billboard mode options
enum class BillboardMode {
    YAxisLocked,    // Doom-style: Sprite rotates only on Y axis (doesn't tilt with camera)
    FullFacing      // Full billboard: Sprite always faces camera completely
};

// Renders billboard sprites that always face the camera
class BillboardRenderer {
public:
    BillboardRenderer();
    ~BillboardRenderer();

    // Initialize renderer with billboard shader
    void init();

    // Draw a single billboard sprite at world position
    void drawSprite(
        const Sprite* sprite,
        const glm::vec3& position,
        const Camera& camera,
        float aspectRatio,
        BillboardMode mode = BillboardMode::YAxisLocked,
        const glm::vec3& scale = glm::vec3(1.0f),
        const glm::vec4& color = glm::vec4(1.0f)
    );

    // For future: batch rendering
    void beginBatch();
    void endBatch();

private:
    void initQuad();
    void cleanup();

    Shader* m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    bool m_initialized;
};
