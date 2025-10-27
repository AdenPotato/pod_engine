#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Forward declarations
class Sprite;
enum class BillboardMode;  // Defined in billboard_renderer.h

// ============================================================================
// Transform Component
// ============================================================================
struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);  // Euler angles (pitch, yaw, roll)
    glm::vec3 scale = glm::vec3(1.0f);

    // Get model matrix
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Yaw
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // Pitch
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Roll
        model = glm::scale(model, scale);
        return model;
    }

    // Get forward vector
    glm::vec3 getForward() const {
        float yaw = glm::radians(rotation.y);
        float pitch = glm::radians(rotation.x);
        return glm::normalize(glm::vec3(
            cos(pitch) * sin(yaw),
            -sin(pitch),
            cos(pitch) * cos(yaw)
        ));
    }

    // Get right vector
    glm::vec3 getRight() const {
        return glm::normalize(glm::cross(getForward(), glm::vec3(0, 1, 0)));
    }

    // Get up vector
    glm::vec3 getUp() const {
        return glm::normalize(glm::cross(getRight(), getForward()));
    }
};

// ============================================================================
// Velocity Component
// ============================================================================
struct Velocity {
    glm::vec3 velocity = glm::vec3(0.0f);
};

// ============================================================================
// Sprite Component
// ============================================================================
struct SpriteComponent {
    Sprite* sprite = nullptr;  // Which sprite frame to render
    glm::vec4 color = glm::vec4(1.0f);  // Tint color
    bool flipX = false;
    bool flipY = false;
};

// ============================================================================
// Billboard Component - How to render the sprite
// Note: BillboardMode enum is defined in billboard_renderer.h
// ============================================================================
struct BillboardComponent {
    BillboardMode mode;  // Default will be set in application code
    float scale = 1.0f;
};

// ============================================================================
// Health Component
// ============================================================================
struct Health {
    int current = 100;
    int maximum = 100;

    bool isDead() const { return current <= 0; }
    float getPercent() const { return (float)current / (float)maximum; }
};




// ============================================================================
// Tags (empty structs used for identification)
// ============================================================================
struct Player {};
struct Enemy {};
struct Item {};
struct Projectile {};
struct Decoration {};
