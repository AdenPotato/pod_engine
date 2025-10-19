#include "application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream>

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

Application::Application(int width, int height, const std::string& title)
    : m_deltaTime(0.0f)
    , m_lastFrame(0.0f)
    , m_firstMouse(true)
    , m_lastX(width / 2.0f)
    , m_lastY(height / 2.0f)
    , m_cursorLocked(true)
    , m_tabKeyPressed(false) {
    init();
    m_window = std::make_unique<Window>(width, height, title);
    m_renderer = std::make_unique<Renderer>();
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    m_shader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    m_imguiLayer = std::make_unique<ImGuiLayer>();

    setupCallbacks();
    m_imguiLayer->init(m_window->getHandle());
    createScene();
}

void Application::init() {
    std::cout << "Pod Engine initialized successfully!" << std::endl;
    std::cout << "Controls: WASD to move, Mouse to look around, ESC to exit" << std::endl;
}

void Application::setupCallbacks() {
    m_window->setFramebufferSizeCallback([this](int width, int height) {
        onFramebufferSize(width, height);
    });

    m_window->setCursorPosCallback([this](double xpos, double ypos) {
        onMouseMove(xpos, ypos);
    });

    m_window->setScrollCallback([this](double xoffset, double yoffset) {
        onMouseScroll(xoffset, yoffset);
    });

    m_window->setCursorMode(GLFW_CURSOR_DISABLED);
}

void Application::createScene() {
    // Create cube mesh once
    Mesh cubeMesh = Mesh::createCube();

    // Create multiple cubes at different positions
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Add cubes to scene
    for (const auto& pos : cubePositions) {
        m_sceneObjects.emplace_back(Mesh::createCube(), Transform(pos));
    }

    std::cout << "Scene created with " << m_sceneObjects.size() << " objects" << std::endl;
}

void Application::run() {
    while (!m_window->shouldClose()) {
        update();

        m_imguiLayer->beginFrame();

        // Disable ImGui input when cursor is locked to camera
        ImGuiIO& io = ImGui::GetIO();
        if (m_cursorLocked) {
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
        } else {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        }

        render();
        renderUI();
        m_imguiLayer->endFrame();

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}

void Application::update() {
    // Update timing
    float currentFrame = static_cast<float>(glfwGetTime());
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    // Process input
    processInput();

    // Update scene object rotations
    for (size_t i = 0; i < m_sceneObjects.size(); ++i) {
        auto& transform = m_sceneObjects[i].second;
        float angle = 20.0f * i + currentFrame * 15.0f;
        transform.rotation = glm::vec3(angle, angle * 0.3f, angle * 0.5f);
    }
}

void Application::render() {
    m_renderer->clear();

    // Draw all scene objects
    for (const auto& [mesh, transform] : m_sceneObjects) {
        m_renderer->drawMesh(mesh, *m_shader, transform.getModelMatrix(), *m_camera, m_window->getAspectRatio());
    }
}

void Application::renderUI() {
    // ImGui Demo Window (for reference)
    // ImGui::ShowDemoWindow();

    // Engine stats window
    ImGui::Begin("Engine Stats");

    // Cursor mode indicator
    if (m_cursorLocked) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Camera Control Mode");
        ImGui::Text("Press TAB to unlock cursor");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "UI Interaction Mode");
        ImGui::Text("Press TAB to lock cursor");
    }
    ImGui::Separator();

    ImGui::Text("FPS: %.1f", 1.0f / m_deltaTime);
    ImGui::Text("Frame Time: %.3f ms", m_deltaTime * 1000.0f);
    ImGui::Separator();

    ImGui::Text("Camera Position");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f",
                m_camera->Position.x,
                m_camera->Position.y,
                m_camera->Position.z);
    ImGui::Separator();

    ImGui::Text("Scene Objects: %zu", m_sceneObjects.size());

    if (ImGui::CollapsingHeader("Controls")) {
        ImGui::BulletText("TAB - Toggle cursor lock");
        ImGui::BulletText("WASD - Move camera (when locked)");
        ImGui::BulletText("Space - Move up (when locked)");
        ImGui::BulletText("Shift - Move down (when locked)");
        ImGui::BulletText("Mouse - Look around (when locked)");
        ImGui::BulletText("ESC - Exit");
    }

    ImGui::End();
}

void Application::processInput() {
    if (m_window->isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window->getHandle(), true);
    }

    // Toggle cursor lock with TAB key (edge-triggered)
    bool tabCurrentlyPressed = m_window->isKeyPressed(GLFW_KEY_TAB);
    if (tabCurrentlyPressed && !m_tabKeyPressed) {
        m_cursorLocked = !m_cursorLocked;

        if (m_cursorLocked) {
            m_window->setCursorMode(GLFW_CURSOR_DISABLED);
            m_firstMouse = true;  // Reset to prevent camera jump
        } else {
            m_window->setCursorMode(GLFW_CURSOR_NORMAL);
        }
    }
    m_tabKeyPressed = tabCurrentlyPressed;

    // Only process camera movement when cursor is locked
    if (m_cursorLocked) {
        if (m_window->isKeyPressed(GLFW_KEY_W))
            m_camera->processKeyboard(FORWARD, m_deltaTime);
        if (m_window->isKeyPressed(GLFW_KEY_S))
            m_camera->processKeyboard(BACKWARD, m_deltaTime);
        if (m_window->isKeyPressed(GLFW_KEY_A))
            m_camera->processKeyboard(LEFT, m_deltaTime);
        if (m_window->isKeyPressed(GLFW_KEY_D))
            m_camera->processKeyboard(RIGHT, m_deltaTime);
        if (m_window->isKeyPressed(GLFW_KEY_SPACE))
            m_camera->processKeyboard(UP, m_deltaTime);
        if (m_window->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
            m_camera->processKeyboard(DOWN, m_deltaTime);
    }
}

void Application::onFramebufferSize(int width, int height) {
    m_renderer->setViewport(0, 0, width, height);
}

void Application::onMouseMove(double xpos, double ypos) {
    // Only process camera movement when cursor is locked
    if (!m_cursorLocked) {
        return;
    }

    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);

    if (m_firstMouse) {
        m_lastX = xposf;
        m_lastY = yposf;
        m_firstMouse = false;
    }

    float xoffset = xposf - m_lastX;
    float yoffset = m_lastY - yposf;

    m_lastX = xposf;
    m_lastY = yposf;

    m_camera->processMouseMovement(xoffset, yoffset);
}

void Application::onMouseScroll(double xoffset, double yoffset) {
    // Only zoom camera when cursor is locked
    if (!m_cursorLocked) {
        return;
    }

    m_camera->processMouseScroll(static_cast<float>(yoffset));
}
