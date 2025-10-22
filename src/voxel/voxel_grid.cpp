#include "voxel_grid.h"
#include <iostream>
#include <cmath>

VoxelGrid::VoxelGrid(int sizeX, int sizeY, int sizeZ)
    : m_size(sizeX, sizeY, sizeZ), m_ssbo(0) {

    // Allocate voxel data
    m_voxels.resize(sizeX * sizeY * sizeZ);

    // Generate SSBO
    glGenBuffers(1, &m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 m_voxels.size() * sizeof(Voxel),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

VoxelGrid::~VoxelGrid() {
    if (m_ssbo != 0) {
        glDeleteBuffers(1, &m_ssbo);
    }
}

void VoxelGrid::initialize() {
    // Create a simple test scene with different colored voxels scaled to grid size

    // Fill the bottom half with ground (green-brown gradient)
    for (int y = 0; y < m_size.y / 2; y++) {
        for (int x = 0; x < m_size.x; x++) {
            for (int z = 0; z < m_size.z; z++) {
                float ratio = (float)y / (m_size.y / 2);
                glm::vec3 color = glm::mix(
                    glm::vec3(0.4f, 0.25f, 0.1f),  // Brown at bottom
                    glm::vec3(0.3f, 0.6f, 0.2f),   // Green at top
                    ratio
                );
                setVoxel(x, y, z, Voxel(1, color));
            }
        }
    }

    // Add some colored structures for variety (scaled to grid size)
    int structureScale = m_size.x / 32;  // Scale structures based on grid size

    // Red pillar
    int pillarHeight = 8 * structureScale;
    int pillarWidth = 3 * structureScale;
    for (int y = m_size.y / 2; y < m_size.y / 2 + pillarHeight; y++) {
        for (int x = 5 * structureScale; x < 5 * structureScale + pillarWidth; x++) {
            for (int z = 5 * structureScale; z < 5 * structureScale + pillarWidth; z++) {
                setVoxel(x, y, z, Voxel(2, glm::vec3(0.8f, 0.2f, 0.2f)));
            }
        }
    }

    // Blue pillar
    int bluePillarHeight = 10 * structureScale;
    for (int y = m_size.y / 2; y < m_size.y / 2 + bluePillarHeight; y++) {
        for (int x = 24 * structureScale; x < 24 * structureScale + pillarWidth; x++) {
            for (int z = 24 * structureScale; z < 24 * structureScale + pillarWidth; z++) {
                setVoxel(x, y, z, Voxel(3, glm::vec3(0.2f, 0.4f, 0.9f)));
            }
        }
    }

    // Yellow sphere
    glm::vec3 sphereCenter(16.0f * structureScale, 20.0f * structureScale, 16.0f * structureScale);
    float sphereRadius = 4.0f * structureScale;
    int sphereMin = static_cast<int>(sphereCenter.y - sphereRadius - 1);
    int sphereMax = static_cast<int>(sphereCenter.y + sphereRadius + 1);
    for (int y = sphereMin; y < sphereMax; y++) {
        for (int x = static_cast<int>(sphereCenter.x - sphereRadius - 1);
             x < static_cast<int>(sphereCenter.x + sphereRadius + 1); x++) {
            for (int z = static_cast<int>(sphereCenter.z - sphereRadius - 1);
                 z < static_cast<int>(sphereCenter.z + sphereRadius + 1); z++) {
                glm::vec3 pos(x + 0.5f, y + 0.5f, z + 0.5f);
                float dist = glm::length(pos - sphereCenter);
                if (dist < sphereRadius) {
                    setVoxel(x, y, z, Voxel(4, glm::vec3(0.9f, 0.9f, 0.2f)));
                }
            }
        }
    }

    // Purple wall
    int wallHeight = 6 * structureScale;
    int wallWidth = 12 * structureScale;
    for (int y = m_size.y / 2; y < m_size.y / 2 + wallHeight; y++) {
        for (int x = 10 * structureScale; x < 10 * structureScale + wallWidth; x++) {
            setVoxel(x, y, 28 * structureScale, Voxel(5, glm::vec3(0.6f, 0.2f, 0.8f)));
        }
    }

    std::cout << "Voxel grid initialized with " << getVoxelCount() << " voxels" << std::endl;

    // Upload to GPU
    uploadToGPU();

    // Verify some voxels were set
    int solidCount = 0;
    for (const auto& voxel : m_voxels) {
        if (voxel.materialType != 0) solidCount++;
    }
    std::cout << "Solid voxels: " << solidCount << " / " << getVoxelCount() << std::endl;
}

void VoxelGrid::setVoxel(int x, int y, int z, const Voxel& voxel) {
    if (x < 0 || x >= m_size.x ||
        y < 0 || y >= m_size.y ||
        z < 0 || z >= m_size.z) {
        return;
    }

    m_voxels[coordToIndex(x, y, z)] = voxel;
}

Voxel VoxelGrid::getVoxel(int x, int y, int z) const {
    if (x < 0 || x >= m_size.x ||
        y < 0 || y >= m_size.y ||
        z < 0 || z >= m_size.z) {
        return Voxel();
    }

    return m_voxels[coordToIndex(x, y, z)];
}

void VoxelGrid::uploadToGPU() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    m_voxels.size() * sizeof(Voxel),
                    m_voxels.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void VoxelGrid::bindSSBO(unsigned int binding) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ssbo);
}

int VoxelGrid::coordToIndex(int x, int y, int z) const {
    return x + m_size.x * (y + m_size.y * z);
}
