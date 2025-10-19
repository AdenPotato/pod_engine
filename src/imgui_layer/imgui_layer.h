#pragma once

#include <GLFW/glfw3.h>

class ImGuiLayer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void init(GLFWwindow* window);
    void shutdown();

    void beginFrame();
    void endFrame();

    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;

private:
    bool m_initialized;
};
