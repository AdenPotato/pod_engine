#include "application.h"
#include "billboard_renderer.h"
#include "sprite.h"
#include "components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream>

Application::~Application() {
    // Destructor defined here so unique_ptr can delete forward-declared types
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

    // Position camera at player eye level (1.7m) above ground plane at origin
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.7f, 0.0f));
    m_camera->MovementSpeed = 5.0f;  // 5 m/s movement speed

    m_shader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    m_imguiLayer = std::make_unique<ImGuiLayer>();

    // Load prototype texture
    m_prototypeTexture = std::make_unique<Texture>();
    if (!m_prototypeTexture->loadFromFile("assets/textures/prototype_square.png", true)) {
        std::cerr << "ERROR: Failed to load prototype texture!" << std::endl;
    } else {
        std::cout << "Texture loaded successfully: "
                  << m_prototypeTexture->getWidth() << "x" << m_prototypeTexture->getHeight()
                  << " (" << m_prototypeTexture->getChannels() << " channels)" << std::endl;
    }

    setupCallbacks();
    m_imguiLayer->init(m_window->getHandle());
    createScene();

    // Initialize billboard sprite system
    m_billboardRenderer = std::make_unique<BillboardRenderer>();
    m_billboardRenderer->init();

    // Load a test sprite
    m_testSprite = std::make_unique<Sprite>();
    if (!m_testSprite->loadFromFile("assets/textures/prototype_square.png")) {
        std::cerr << "Failed to load test sprite!" << std::endl;
    }

    // Create a test entity with EnTT
    auto testEntity = m_registry.create();

    // Add Transform component
    auto& transform = m_registry.emplace<Transform>(testEntity);
    transform.position = glm::vec3(0.0f, 1.0f, -3.0f);

    // Add SpriteComponent
    auto& spriteComp = m_registry.emplace<SpriteComponent>(testEntity);
    spriteComp.sprite = m_testSprite.get();

    // Add BillboardComponent
    auto& billboardComp = m_registry.emplace<BillboardComponent>(testEntity);
    billboardComp.mode = BillboardMode::YAxisLocked;
    billboardComp.scale = 1.0f;

    std::cout << "Created test entity with ID: " << static_cast<uint32_t>(testEntity) << std::endl;
    std::cout << "Camera starting position: (" << m_camera->Position.x << ", "
              << m_camera->Position.y << ", " << m_camera->Position.z << ")" << std::endl;
}

void Application::init() {
    std::cout << "Pod Engine initialized successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Left Click - Destroy voxels" << std::endl;
    std::cout << "  TAB - Toggle cursor lock" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
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
    // Create simple ground plane for testing
    // 20x20m ground plane centered at origin
    // Texture is 512x512 with 4x4 grid, so entire texture = 4x4 meters
    // Therefore: UV scale = distance_in_meters / 4.0

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Ground plane (y = 0) - facing up
    // 20m x 20m = 5 texture repeats (20 / 4 = 5)
    vertices.push_back({{-10.0f, 0.0f, 10.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 5.0f}});
    vertices.push_back({{10.0f, 0.0f, 10.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {5.0f, 5.0f}});
    vertices.push_back({{10.0f, 0.0f, -10.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {5.0f, 0.0f}});
    vertices.push_back({{-10.0f, 0.0f, -10.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    indices.insert(indices.end(), {0, 1, 2, 2, 3, 0});

    m_testLevel = std::make_unique<Mesh>(vertices, indices);

    std::cout << "Test level created: 20x20m ground plane with " << vertices.size() << " vertices" << std::endl;
    std::cout << "Texture scale: 512x512 texture with 4x4 grid = 4m per texture repeat" << std::endl;
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
}

void Application::render() {
    m_renderer->clear();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::mat4(1.0f);  // Identity matrix for level

    // Render ground plane
    if (m_prototypeTexture && m_prototypeTexture->isLoaded()) {
        m_renderer->drawMesh(*m_testLevel, *m_shader, model, *m_camera,
                            m_window->getAspectRatio(), m_prototypeTexture.get());
    } else {
        m_renderer->drawMesh(*m_testLevel, *m_shader, model, *m_camera,
                            m_window->getAspectRatio());
    }

    // Render all entities with sprites using EnTT view
    auto view = m_registry.view<Transform, SpriteComponent, BillboardComponent>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& sprite = view.get<SpriteComponent>(entity);
        auto& billboard = view.get<BillboardComponent>(entity);

        if (sprite.sprite && sprite.sprite->isLoaded()) {
            m_billboardRenderer->drawSprite(
                sprite.sprite,
                transform.position,
                *m_camera,
                m_window->getAspectRatio(),
                billboard.mode,
                transform.scale,
                sprite.color
            );
        }
    }
}

void Application::renderUI() {
    // Engine stats window
    ImGui::Begin("Pod Engine - Doom Looter Shooter");

    // Cursor mode indicator
    if (m_cursorLocked) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "FPS Control Mode");
        ImGui::Text("Press TAB to unlock cursor");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "UI Mode");
        ImGui::Text("Press TAB to lock cursor");
    }
    ImGui::Separator();

    ImGui::Text("FPS: %.1f", 1.0f / m_deltaTime);
    ImGui::Text("Frame Time: %.3f ms", m_deltaTime * 1000.0f);
    ImGui::Separator();

    ImGui::Text("Player Position");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f",
                m_camera->Position.x,
                m_camera->Position.y,
                m_camera->Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", m_camera->Yaw, m_camera->Pitch);
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Controls")) {
        ImGui::BulletText("TAB - Toggle cursor lock");
        ImGui::BulletText("WASD - Move (5 m/s)");
        ImGui::BulletText("Space - Move up");
        ImGui::BulletText("Shift - Move down");
        ImGui::BulletText("Mouse - Look around");
        ImGui::BulletText("ESC - Exit");
    }

    if (ImGui::CollapsingHeader("Level Info")) {
        ImGui::Text("Test Level: 10x10x3m room");
        ImGui::Text("Texture: prototype_square.png");
        ImGui::Text("Scale: 512x512px = 1x1m");
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
