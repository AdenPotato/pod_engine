#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices)
    : m_VAO(0), m_VBO(0), m_EBO(0), m_vertexCount(vertices.size()), m_indexCount(0), m_useIndices(false) {
    setupMesh(vertices, {});
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_VAO(0), m_VBO(0), m_EBO(0), m_vertexCount(vertices.size()), m_indexCount(indices.size()), m_useIndices(true) {
    setupMesh(vertices, indices);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_VAO(other.m_VAO)
    , m_VBO(other.m_VBO)
    , m_EBO(other.m_EBO)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_useIndices(other.m_useIndices) {
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();

        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_useIndices = other.m_useIndices;

        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    if (m_useIndices && !indices.empty()) {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Texture coordinate attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(m_VAO);
    if (m_useIndices) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
}

Mesh Mesh::createCube() {
    std::vector<Vertex> vertices = {
        // Front face (Red)
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back face (Green)
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

        // Left face (Blue)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

        // Right face (Yellow)
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

        // Top face (Magenta)
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

        // Bottom face (Cyan)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };

    return Mesh(vertices, indices);
}

Mesh Mesh::createPlane(float size) {
    std::vector<Vertex> vertices = {
        {{-size, 0.0f, -size}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ size, 0.0f, -size}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ size, 0.0f,  size}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-size, 0.0f,  size}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };

    return Mesh(vertices, indices);
}
