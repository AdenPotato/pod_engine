#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// Voxel structure: stores material type and color
// Must match std430 layout in shader (16-byte alignment for vec3)
struct Voxel {
    uint32_t materialType;  // 0 = air, 1+ = different materials
    float colorR;           // Red component
    float colorG;           // Green component
    float colorB;           // Blue component

    Voxel() : materialType(0), colorR(0.0f), colorG(0.0f), colorB(0.0f) {}
    Voxel(uint32_t type, const glm::vec3& col)
        : materialType(type), colorR(col.r), colorG(col.g), colorB(col.b) {}

    glm::vec3 getColor() const { return glm::vec3(colorR, colorG, colorB); }
};

class VoxelGrid {
public:
    VoxelGrid(int sizeX = 32, int sizeY = 32, int sizeZ = 32);
    ~VoxelGrid();

    // Disable copying
    VoxelGrid(const VoxelGrid&) = delete;
    VoxelGrid& operator=(const VoxelGrid&) = delete;

    // Initialize the voxel grid with test data
    void initialize();

    // Set a voxel at position
    void setVoxel(int x, int y, int z, const Voxel& voxel);

    // Get a voxel at position
    Voxel getVoxel(int x, int y, int z) const;

    // Upload voxel data to GPU SSBO
    void uploadToGPU();

    // Bind the SSBO for use in shaders
    void bindSSBO(unsigned int binding = 0) const;

    // Get grid dimensions
    glm::ivec3 getSize() const { return m_size; }
    int getSizeX() const { return m_size.x; }
    int getSizeY() const { return m_size.y; }
    int getSizeZ() const { return m_size.z; }

    // Get total voxel count
    int getVoxelCount() const { return m_size.x * m_size.y * m_size.z; }

    // Get the SSBO handle
    GLuint getSSBO() const { return m_ssbo; }

private:
    glm::ivec3 m_size;
    std::vector<Voxel> m_voxels;
    GLuint m_ssbo;  // Shader Storage Buffer Object

    // Helper to convert 3D coordinates to 1D index
    int coordToIndex(int x, int y, int z) const;
};
