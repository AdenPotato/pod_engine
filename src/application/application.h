#pragma once

#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "imgui_layer.h"
#include <entt/entt.hpp>
#include <memory>
#include <vector>

// Forward declarations
class BillboardRenderer;
class Sprite;

class Application {
public:
    Application(int width = 1280, int height = 720, const std::string& title = "Pod Engine");
    ~Application();  // Need to define in .cpp for forward-declared unique_ptr members

    void run();

private:
    // Core systems
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<ImGuiLayer> m_imguiLayer;

    // EnTT ECS registry
    entt::registry m_registry;

    // Test level for Doom-style game
    std::unique_ptr<Mesh> m_testLevel;
    std::unique_ptr<Texture> m_prototypeTexture;

    // Billboard sprite system
    std::unique_ptr<class BillboardRenderer> m_billboardRenderer;
    std::unique_ptr<class Sprite> m_testSprite;

    // Timing
    float m_deltaTime;
    float m_lastFrame;

    // Input state
    bool m_firstMouse;
    float m_lastX;
    float m_lastY;
    bool m_cursorLocked;
    bool m_tabKeyPressed;  // Track TAB key state to prevent repeat toggles

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
};
