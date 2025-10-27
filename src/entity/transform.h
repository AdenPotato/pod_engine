#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Transform component (built-in to all entities)
struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;  // Euler angles (degrees)
    glm::vec3 scale;

    Transform(const glm::vec3& pos = glm::vec3(0.0f),
              const glm::vec3& rot = glm::vec3(0.0f),
              const glm::vec3& scl = glm::vec3(1.0f))
        : position(pos)
        , rotation(rot)
        , scale(scl)
    {}

    // Get model matrix for rendering
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }

    // Get forward vector (useful for sprite facing direction)
    glm::vec3 getForward() const {
        float yaw = glm::radians(rotation.y);
        return glm::normalize(glm::vec3(
            sin(yaw),
            0.0f,
            -cos(yaw)
        ));
    }

    // Get right vector
    glm::vec3 getRight() const {
        float yaw = glm::radians(rotation.y);
        return glm::normalize(glm::vec3(
            cos(yaw),
            0.0f,
            sin(yaw)
        ));
    }
};
