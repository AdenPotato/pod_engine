#pragma once

#include "imgui.h"
#include "podpch.h"
#include "Core/Application/Layer.h"
#include "GLFW/glfw3.h"


class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void OnUpdate();
    void OnEvent(Event& event);
    void OnAttach();
    void OnDetach();

    ImGuiKey ToImGuiKey(int key);
private:

    float m_Time = 0.0f;
};
