#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex(const glm::vec3& pos = glm::vec3(0.0f),
           const glm::vec3& col = glm::vec3(1.0f),
           const glm::vec3& norm = glm::vec3(0.0f, 1.0f, 0.0f),
           const glm::vec2& tex = glm::vec2(0.0f))
        : position(pos), color(col), normal(norm), texCoords(tex) {}
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Prevent copying
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow moving
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw() const;

    // Get VAO handle
    unsigned int getVAO() const { return m_VAO; }

    // Factory methods for common shapes
    static Mesh createCube();
    static Mesh createPlane(float size = 1.0f);

private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
    unsigned int m_vertexCount;
    unsigned int m_indexCount;
    bool m_useIndices;

    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void cleanup();
};
