//
// Created by Aden on 10/28/2025.
//

#include "Application.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Application/Layer.h"
#include "Core/Application/LayerStack.h"
#include "Core/Events/KeyEvent.h"
#include "glad/glad.h"
#include "Core/Input/Input.h"

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;


    Log::Init();
    CORE_LOGGER_INFO("Logging Initialized");
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));



}
Application::~Application()
{

}

void Application::run()
{
    PushOverlay(new ImGuiLayer);

    while(m_Running)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (Layer *layer : m_LayerStack) layer->OnUpdate();

        auto[x, y] = Input::GetMousePosition();
        //CORE_LOGGER_TRACE("{0}, {1}", x, y);
        if (Input::IsMouseButtonPressed(POD_MOUSE_BUTTON_LEFT))
        {
            CORE_LOGGER_TRACE("MY BALLS ARE HUNGRY");
        }



        m_Window->OnUpdate();
    }
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClosed));

    //CORE_LOGGER_TRACE("{0}", event);

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(event);
        if (event.Handled())
            break;
    }

}

bool Application::OnWindowClosed(WindowCloseEvent &e)
{
    m_Running = false;
    return true;
}

void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}
void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverLay(layer);
    layer->OnAttach();
}


