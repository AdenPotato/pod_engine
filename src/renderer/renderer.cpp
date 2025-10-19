#include "renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    setupRenderState();
}

void Renderer::setupRenderState() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::drawMesh(const Mesh& mesh, const Shader& shader, const glm::mat4& model, const Camera& camera, float aspectRatio) {
    shader.use();

    // Set transformation matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);

    mesh.draw();
}

void Renderer::enableDepthTest(bool enable) {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::enableWireframe(bool enable) {
    if (enable) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
