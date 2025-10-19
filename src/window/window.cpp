#include "window.h"
#include <iostream>
#include <stdexcept>

// Initialize static members
std::function<void(int, int)> Window::s_framebufferSizeCallback;
std::function<void(double, double)> Window::s_cursorPosCallback;
std::function<void(double, double)> Window::s_scrollCallback;

Window::Window(int width, int height, const std::string& title)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_title(title) {
    initGLFW();
    initGLAD();
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void Window::initGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    // Set GLFW callbacks
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
}

void Window::initGLAD() {
    if (!gladLoadGLLoader([](const char* name) { return (void*)glfwGetProcAddress(name); })) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::setFramebufferSizeCallback(std::function<void(int, int)> callback) {
    s_framebufferSizeCallback = callback;
}

void Window::setCursorPosCallback(std::function<void(double, double)> callback) {
    s_cursorPosCallback = callback;
}

void Window::setScrollCallback(std::function<void(double, double)> callback) {
    s_scrollCallback = callback;
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Window::setCursorMode(int mode) {
    glfwSetInputMode(m_window, GLFW_CURSOR, mode);
}

// Static callback implementations
void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (s_framebufferSizeCallback) {
        s_framebufferSizeCallback(width, height);
    }
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_cursorPosCallback) {
        s_cursorPosCallback(xpos, ypos);
    }
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (s_scrollCallback) {
        s_scrollCallback(xoffset, yoffset);
    }
}
