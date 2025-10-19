#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "mesh.h"
#include "camera.h"

class Renderer {
public:
    Renderer();
    ~Renderer() = default;

    void clear(const glm::vec4& color = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f));
    void setViewport(int x, int y, int width, int height);

    // Rendering methods
    void drawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model, const Camera& camera, float aspectRatio);

    // State management
    void enableDepthTest(bool enable = true);
    void enableWireframe(bool enable = true);

private:
    void setupRenderState();
};
