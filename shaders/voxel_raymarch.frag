#version 460 core

in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

// Voxel structure matching C++ side
struct Voxel {
    uint materialType;
    float colorR;
    float colorG;
    float colorB;
};

// Shader Storage Buffer for voxels
layout(std430, binding = 0) buffer VoxelBuffer {
    Voxel voxels[];
};

// Uniforms
uniform vec3 gridSize;
uniform vec3 cameraPos;
uniform float voxelSize;  // Size of each voxel in world space

// Convert 3D coordinates to 1D buffer index
int coordToIndex(ivec3 coord) {
    ivec3 gridSizeInt = ivec3(gridSize);
    return coord.x + gridSizeInt.x * (coord.y + gridSizeInt.y * coord.z);
}

// Check if voxel coordinate is within bounds
bool inBounds(ivec3 coord) {
    ivec3 gridSizeInt = ivec3(gridSize);
    return coord.x >= 0 && coord.x < gridSizeInt.x &&
           coord.y >= 0 && coord.y < gridSizeInt.y &&
           coord.z >= 0 && coord.z < gridSizeInt.z;
}

// Get voxel at coordinate
Voxel getVoxel(ivec3 coord) {
    if (!inBounds(coord)) {
        Voxel empty;
        empty.materialType = 0;
        empty.colorR = 0.0;
        empty.colorG = 0.0;
        empty.colorB = 0.0;
        return empty;
    }
    return voxels[coordToIndex(coord)];
}

// Helper to get color from voxel
vec3 getVoxelColor(Voxel v) {
    return vec3(v.colorR, v.colorG, v.colorB);
}

// Simple lighting calculation
vec3 calculateLighting(vec3 color, vec3 normal, vec3 viewDir) {
    // Light direction (from top-right-front)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));

    // Ambient
    vec3 ambient = 0.3 * color;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular (simple)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = 0.3 * spec * vec3(1.0);

    return ambient + diffuse + specular;
}

// DDA-based voxel raymarching
vec4 raymarch(vec3 rayOrigin, vec3 rayDir) {
    // Normalize ray direction to avoid precision issues
    rayDir = normalize(rayDir);

    // Add small epsilon to avoid division by zero
    const float epsilon = 0.0001;
    vec3 rayDirSafe = rayDir;
    if (abs(rayDirSafe.x) < epsilon) rayDirSafe.x = epsilon;
    if (abs(rayDirSafe.y) < epsilon) rayDirSafe.y = epsilon;
    if (abs(rayDirSafe.z) < epsilon) rayDirSafe.z = epsilon;

    // Calculate grid bounds in world space
    vec3 gridMin = vec3(0.0);
    vec3 gridMax = vec3(gridSize) * voxelSize;

    // Ray-box intersection with grid bounds
    vec3 tMin = (gridMin - rayOrigin) / rayDirSafe;
    vec3 tMax = (gridMax - rayOrigin) / rayDirSafe;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    // Check if ray intersects grid
    if (tNear > tFar || tFar < 0.0) {
        // No intersection - return sky color
        vec3 skyColor = mix(vec3(0.5, 0.6, 0.7), vec3(0.3, 0.4, 0.6), rayDir.y * 0.5 + 0.5);
        return vec4(skyColor, 1.0);
    }

    // Start slightly inside the grid to avoid boundary issues
    float t = max(tNear + epsilon, 0.0);
    vec3 pos = rayOrigin + rayDirSafe * t;

    // DDA setup - convert to voxel coordinates
    ivec3 voxelCoord = ivec3(floor(pos / voxelSize));

    // Clamp to grid bounds
    ivec3 gridSizeInt = ivec3(gridSize);
    voxelCoord = clamp(voxelCoord, ivec3(0), gridSizeInt - 1);

    // DDA step direction
    ivec3 step = ivec3(sign(rayDirSafe));

    // Distance to next voxel boundary along each axis
    vec3 deltaDist = abs(voxelSize / rayDirSafe);

    // Calculate initial sideDist more carefully
    vec3 voxelPos = vec3(voxelCoord) * voxelSize;
    vec3 sideDist;

    for (int axis = 0; axis < 3; axis++) {
        if (rayDirSafe[axis] > 0.0) {
            sideDist[axis] = ((voxelPos[axis] + voxelSize) - pos[axis]) / rayDirSafe[axis];
        } else if (rayDirSafe[axis] < 0.0) {
            sideDist[axis] = (voxelPos[axis] - pos[axis]) / rayDirSafe[axis];
        } else {
            sideDist[axis] = 1e10;  // Very large number
        }
    }

    // Normal tracking for lighting
    vec3 normal = vec3(0.0, 1.0, 0.0);

    // Maximum steps to prevent infinite loops
    const int maxSteps = 256;

    for (int i = 0; i < maxSteps; i++) {
        // Check current voxel
        if (inBounds(voxelCoord)) {
            Voxel voxel = getVoxel(voxelCoord);

            if (voxel.materialType != 0u) {
                // Hit a solid voxel
                vec3 hitPos = rayOrigin + rayDirSafe * (t + min(min(sideDist.x, sideDist.y), sideDist.z));
                vec3 viewDir = normalize(rayOrigin - hitPos);
                vec3 voxelColor = getVoxelColor(voxel);
                vec3 litColor = calculateLighting(voxelColor, normal, viewDir);

                // Simple fog based on distance
                float dist = length(hitPos - rayOrigin);
                float fogFactor = exp(-dist * 0.03);
                vec3 fogColor = vec3(0.5, 0.6, 0.7);
                vec3 finalColor = mix(fogColor, litColor, fogFactor);

                return vec4(finalColor, 1.0);
            }
        } else {
            // Outside grid bounds
            break;
        }

        // DDA step to next voxel - find which boundary is closest
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            // Step in X
            sideDist.x += deltaDist.x;
            voxelCoord.x += step.x;
            normal = vec3(-float(step.x), 0.0, 0.0);
        } else if (sideDist.y < sideDist.z) {
            // Step in Y
            sideDist.y += deltaDist.y;
            voxelCoord.y += step.y;
            normal = vec3(0.0, -float(step.y), 0.0);
        } else {
            // Step in Z
            sideDist.z += deltaDist.z;
            voxelCoord.z += step.z;
            normal = vec3(0.0, 0.0, -float(step.z));
        }
    }

    // No hit - return sky color
    vec3 skyColor = mix(vec3(0.5, 0.6, 0.7), vec3(0.3, 0.4, 0.6), rayDir.y * 0.5 + 0.5);
    return vec4(skyColor, 1.0);
}

void main() {
    // Calculate ray direction
    vec3 rayDir = normalize(farPoint - nearPoint);
    vec3 rayOrigin = cameraPos;

    // Perform raymarching
    vec4 color = raymarch(rayOrigin, rayDir);
    FragColor = color;
}
