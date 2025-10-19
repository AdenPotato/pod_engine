#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // Prevent copying
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();
    GLFWwindow* getHandle() const { return m_window; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

    // Callback setters
    void setFramebufferSizeCallback(std::function<void(int, int)> callback);
    void setCursorPosCallback(std::function<void(double, double)> callback);
    void setScrollCallback(std::function<void(double, double)> callback);

    // Input methods
    bool isKeyPressed(int key) const;
    void setCursorMode(int mode);

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;

    // Static callback storage (needed for GLFW C-style callbacks)
    static std::function<void(int, int)> s_framebufferSizeCallback;
    static std::function<void(double, double)> s_cursorPosCallback;
    static std::function<void(double, double)> s_scrollCallback;

    // Static GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    void initGLFW();
    void initGLAD();
};
