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
    , m_tabKeyPressed(false)
    , m_leftMousePressed(false)
    , m_destructionRadius(1.2f) {
    init();
    m_window = std::make_unique<Window>(width, height, title);
    m_renderer = std::make_unique<Renderer>();
    m_camera = std::make_unique<Camera>(glm::vec3(6.4f, 8.0f, 20.0f));
    m_shader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    m_imguiLayer = std::make_unique<ImGuiLayer>();

    // Initialize voxel system with higher resolution (128x128x128 for smaller voxels)
    m_voxelGrid = std::make_unique<VoxelGrid>(128, 128, 128);
    m_raymarchShader = std::make_unique<Shader>("shaders/voxel_raymarch.vert", "shaders/voxel_raymarch.frag");
    m_teardownShader = std::make_unique<Shader>("shaders/voxel_teardown.comp");

    // Create fullscreen quad for raymarching
    std::vector<Vertex> quadVertices = {
        {{-1.0f, -1.0f, 0.0f}, {}, {}, {}},
        {{ 1.0f, -1.0f, 0.0f}, {}, {}, {}},
        {{ 1.0f,  1.0f, 0.0f}, {}, {}, {}},
        {{-1.0f,  1.0f, 0.0f}, {}, {}, {}}
    };
    std::vector<unsigned int> quadIndices = {0, 1, 2, 2, 3, 0};
    m_fullscreenQuad = std::make_unique<Mesh>(quadVertices, quadIndices);

    setupCallbacks();
    m_imguiLayer->init(m_window->getHandle());
    createScene();

    // Initialize voxel grid with test scene
    m_voxelGrid->initialize();

    std::cout << "Camera starting position: (" << m_camera->Position.x << ", "
              << m_camera->Position.y << ", " << m_camera->Position.z << ")" << std::endl;
    std::cout << "Voxel grid size: " << m_voxelGrid->getSizeX() << "x"
              << m_voxelGrid->getSizeY() << "x" << m_voxelGrid->getSizeZ() << std::endl;
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
    // Voxel scene is now created in VoxelGrid::initialize()
    std::cout << "Voxel scene initialized" << std::endl;
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

    // Handle voxel destruction
    handleVoxelDestruction();
}

void Application::render() {
    m_renderer->clear();

    // Disable depth test for fullscreen quad raymarching
    glDisable(GL_DEPTH_TEST);

    // Bind voxel SSBO
    m_voxelGrid->bindSSBO(0);

    // Render voxels using raymarching
    m_raymarchShader->use();

    // Set uniforms
    glm::ivec3 gridSize = m_voxelGrid->getSize();
    m_raymarchShader->setVec3("gridSize", glm::vec3(gridSize));
    m_raymarchShader->setVec3("cameraPos", m_camera->Position);
    m_raymarchShader->setFloat("voxelSize", 0.1f);  // Small voxels for Teardown-like appearance
    m_raymarchShader->setMat4("view", m_camera->getViewMatrix());
    m_raymarchShader->setMat4("projection", m_camera->getProjectionMatrix(m_window->getAspectRatio()));

    // Draw fullscreen quad
    glBindVertexArray(m_fullscreenQuad->getVAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}

void Application::renderUI() {
    // ImGui Demo Window (for reference)
    // ImGui::ShowDemoWindow();

    // Engine stats window
    ImGui::Begin("Voxel Teardown System");

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

    ImGui::Text("Voxel Grid: %dx%dx%d",
                m_voxelGrid->getSizeX(),
                m_voxelGrid->getSizeY(),
                m_voxelGrid->getSizeZ());
    ImGui::Text("Total Voxels: %d", m_voxelGrid->getVoxelCount());
    ImGui::Separator();

    ImGui::Text("Destruction Settings");
    ImGui::SliderFloat("Radius", &m_destructionRadius, 0.4f, 4.0f);

    if (ImGui::CollapsingHeader("Controls")) {
        ImGui::BulletText("TAB - Toggle cursor lock");
        ImGui::BulletText("WASD - Move camera (when locked)");
        ImGui::BulletText("Space - Move up (when locked)");
        ImGui::BulletText("Shift - Move down (when locked)");
        ImGui::BulletText("Mouse - Look around (when locked)");
        ImGui::BulletText("Left Click - Destroy voxels (when locked)");
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
// Temporary file - will append this to application.cpp
void Application::handleVoxelDestruction() {
    // Only allow destruction when cursor is locked
    if (!m_cursorLocked) {
        return;
    }

    // Check for left mouse button click (edge-triggered)
    bool leftMouseCurrentlyPressed = glfwGetMouseButton(m_window->getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (leftMouseCurrentlyPressed && !m_leftMousePressed) {
        // Mouse button just pressed - perform destruction

        // Calculate ray from camera
        glm::vec3 rayOrigin = m_camera->Position;
        glm::vec3 rayDir = m_camera->Front;

        // Raycast to find intersection point with voxel grid
        // We'll shoot a ray and find where it first hits a solid voxel

        float voxelSize = 0.1f;  // Must match the voxelSize in render()
        glm::vec3 gridMin = glm::vec3(0.0f);
        glm::vec3 gridMax = glm::vec3(m_voxelGrid->getSize()) * voxelSize;

        // Ray-box intersection with grid bounds
        glm::vec3 tMin = (gridMin - rayOrigin) / rayDir;
        glm::vec3 tMax = (gridMax - rayOrigin) / rayDir;
        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);
        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        if (tNear <= tFar && tFar >= 0.0f) {
            // Ray intersects grid - march through voxels to find first solid voxel
            float t = glm::max(tNear, 0.0f);
            glm::vec3 pos = rayOrigin + rayDir * t;

            // DDA traversal
            glm::ivec3 voxelCoord = glm::ivec3(glm::floor(pos / voxelSize));
            glm::vec3 deltaDist = glm::abs(glm::vec3(voxelSize) / rayDir);
            glm::ivec3 step = glm::ivec3(glm::sign(rayDir));
            glm::vec3 sideDist = (glm::sign(rayDir) * (glm::vec3(voxelCoord) - pos / voxelSize) + (glm::sign(rayDir) * 0.5f) + 0.5f) * deltaDist;

            const int maxSteps = 128;
            bool foundVoxel = false;
            glm::vec3 destructionCenter;

            for (int i = 0; i < maxSteps; i++) {
                // Check if current voxel is solid
                if (voxelCoord.x >= 0 && voxelCoord.x < m_voxelGrid->getSizeX() &&
                    voxelCoord.y >= 0 && voxelCoord.y < m_voxelGrid->getSizeY() &&
                    voxelCoord.z >= 0 && voxelCoord.z < m_voxelGrid->getSizeZ()) {

                    Voxel voxel = m_voxelGrid->getVoxel(voxelCoord.x, voxelCoord.y, voxelCoord.z);
                    if (voxel.materialType != 0) {
                        // Found solid voxel - set destruction center
                        destructionCenter = glm::vec3(voxelCoord) + glm::vec3(0.5f);
                        foundVoxel = true;
                        break;
                    }
                } else {
                    // Outside grid
                    break;
                }

                // Step to next voxel
                if (sideDist.x < sideDist.y) {
                    if (sideDist.x < sideDist.z) {
                        sideDist.x += deltaDist.x;
                        voxelCoord.x += step.x;
                    } else {
                        sideDist.z += deltaDist.z;
                        voxelCoord.z += step.z;
                    }
                } else {
                    if (sideDist.y < sideDist.z) {
                        sideDist.y += deltaDist.y;
                        voxelCoord.y += step.y;
                    } else {
                        sideDist.z += deltaDist.z;
                        voxelCoord.z += step.z;
                    }
                }
            }

            if (foundVoxel) {
                std::cout << "Found target voxel at: (" << destructionCenter.x << ", "
                          << destructionCenter.y << ", " << destructionCenter.z
                          << ") with radius: " << m_destructionRadius << std::endl;

                // Dispatch compute shader to destroy voxels
                m_voxelGrid->bindSSBO(0);
                m_teardownShader->use();
                m_teardownShader->setVec3("gridSize", glm::vec3(m_voxelGrid->getSize()));
                m_teardownShader->setVec3("destructionCenter", destructionCenter);
                m_teardownShader->setFloat("destructionRadius", m_destructionRadius);
                m_teardownShader->setInt("destructionType", 0);  // 0 = sphere

                // Dispatch compute shader (128x128x128 grid with 8x8x8 work groups = 16x16x16 dispatches)
                m_teardownShader->dispatch(16, 16, 16);

                // Memory barrier to ensure compute shader finishes
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

                std::cout << "Compute shader dispatched successfully" << std::endl;
            } else {
                std::cout << "No voxel found in ray path" << std::endl;
            }
        }
    }

    m_leftMousePressed = leftMouseCurrentlyPressed;
}
