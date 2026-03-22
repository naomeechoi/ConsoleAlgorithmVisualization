#pragma once
#include "Common/Common.h"
#include <Windows.h>

namespace RenderEngine
{
    class NAOMI_API MouseInput
    {
    public:
        MouseInput();
        ~MouseInput() = default;
        void Update();
        float GetYawDelta();
        float GetPitchDelta();
        void PostUpdate();

    private:
        const POINT& GetMousePos() const;

    private:
        bool isDragging = false;
        POINT lastPos;
        POINT currentPos;
        float yawDelta = 0.0f;
        float pitchDelta = 0.0f;
    };
}

