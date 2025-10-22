#pragma once

#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "imgui_layer.h"
#include "voxel_grid.h"
#include <memory>
#include <vector>

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform(const glm::vec3& pos = glm::vec3(0.0f),
              const glm::vec3& rot = glm::vec3(0.0f),
              const glm::vec3& scl = glm::vec3(1.0f))
        : position(pos), rotation(rot), scale(scl) {}

    glm::mat4 getModelMatrix() const;
};

class Application {
public:
    Application(int width = 1280, int height = 720, const std::string& title = "Pod Engine");
    ~Application() = default;

    void run();

private:
    // Core systems
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<ImGuiLayer> m_imguiLayer;

    // Voxel system
    std::unique_ptr<VoxelGrid> m_voxelGrid;
    std::unique_ptr<Shader> m_raymarchShader;
    std::unique_ptr<Shader> m_teardownShader;
    std::unique_ptr<Mesh> m_fullscreenQuad;

    // Scene objects (legacy - will be removed)
    std::vector<std::pair<Mesh, Transform>> m_sceneObjects;

    // Timing
    float m_deltaTime;
    float m_lastFrame;

    // Input state
    bool m_firstMouse;
    float m_lastX;
    float m_lastY;
    bool m_cursorLocked;
    bool m_tabKeyPressed;  // Track TAB key state to prevent repeat toggles
    bool m_leftMousePressed;  // Track left mouse button state

    // Voxel interaction
    float m_destructionRadius;

    // Methods
    void init();
    void setupCallbacks();
    void createScene();
    void update();
    void render();
    void renderUI();
    void processInput();

    // Callbacks
    void onFramebufferSize(int width, int height);
    void onMouseMove(double xpos, double ypos);
    void onMouseScroll(double xoffset, double yoffset);

    // Voxel interaction
    void handleVoxelDestruction();
};
