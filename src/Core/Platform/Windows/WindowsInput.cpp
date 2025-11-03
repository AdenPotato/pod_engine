#include "podpch.h"
#include "WindowsInput.h"
#include "Core/Core.h"
#include "Core/Application/Application.h"
#include <utility>

#include <GLFW/glfw3.h>

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
    auto window = static_cast<GLFWwindow*>(Application::GetApp().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
    auto window = static_cast<GLFWwindow*>(Application::GetApp().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}
float WindowsInput::GetMouseXImpl()
{
    auto[x, y] = GetMousePositionImpl();
    return x;
}
float WindowsInput::GetMouseYImpl()
{
    auto[x, y] = GetMousePositionImpl();
    return y;
}


std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
    auto window = static_cast<GLFWwindow*>(Application::GetApp().GetWindow().GetNativeWindow());
    double Xpos, Ypos;
    glfwGetCursorPos(window, &Xpos, &Ypos);

    return { (float)Xpos, (float)Ypos };
}

