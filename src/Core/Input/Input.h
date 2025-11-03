#pragma once

#include "podpch.h"
#include "Core/Core.h"

class Input
{
public:
    //keyboard stuff
    inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

    //mouse stuff
    inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
    inline static  float GetMouseX() { return s_Instance->GetMouseXImpl(); }
    inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
    inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

protected:
    //keyboard
    virtual bool IsKeyPressedImpl(int keycode) = 0;

    //mouse
    virtual bool IsMouseButtonPressedImpl(int button) = 0;
    virtual float GetMouseXImpl() = 0;
    virtual float GetMouseYImpl() = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;



private:
    static Input* s_Instance;
};