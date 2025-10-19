#include "imgui_layer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

ImGuiLayer::ImGuiLayer()
    : m_initialized(false) {
}

ImGuiLayer::~ImGuiLayer() {
    if (m_initialized) {
        shutdown();
    }
}

void ImGuiLayer::init(GLFWwindow* window) {
    if (m_initialized) {
        std::cerr << "ImGui already initialized!" << std::endl;
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    m_initialized = true;
    std::cout << "ImGui initialized successfully" << std::endl;
}

void ImGuiLayer::shutdown() {
    if (!m_initialized) {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_initialized = false;
}

void ImGuiLayer::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiLayer::wantCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiLayer::wantCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}
