#pragma once
#include "podpch.h"
#include "Core/Core.h"
#include "Core/Input/Input.h"

class WindowsInput : public Input
{
protected:
    virtual bool IsKeyPressedImpl(int keycode) override;
    virtual bool IsMouseButtonPressedImpl(int button) override;
    virtual float GetMouseXImpl() override;
    virtual float GetMouseYImpl() override;

    virtual std::pair<float, float> GetMousePositionImpl() override;
};

