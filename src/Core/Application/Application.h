//
// Created by Aden on 10/28/2025.
//
#pragma once

#include "Layer.h"
#include "LayerStack.h"
#include "podpch.h"
#include "Core/Core.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Window/Window.h"


class Application
{

public:
    Application();
    ~Application();

    void run();

    void OnEvent(Event& event);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);


    inline static Application& GetApp() { return *s_Instance; }
    inline Window& GetWindow() { return *m_Window; }

private:
    bool OnWindowClosed(WindowCloseEvent& e);

    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    LayerStack m_LayerStack;

private:
    static Application *s_Instance;
};
