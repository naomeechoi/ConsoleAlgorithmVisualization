#include "MouseInput.h"

namespace RenderEngine
{
    MouseInput::MouseInput()
    {
        currentPos = GetMousePos();
        lastPos = currentPos;
    }

    void MouseInput::Update()
    {
        currentPos = GetMousePos();

        bool isCurrentlyisDragging = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
        if (isCurrentlyisDragging && !isDragging)
        {
            lastPos = currentPos;
        }

        isDragging = isCurrentlyisDragging;
    }

    float MouseInput::GetYawDelta()
    {
        if (!isDragging)
            return 0.0f;
        return float(currentPos.x - lastPos.x);
    }

    float MouseInput::GetPitchDelta()
    {
        if (!isDragging)
            return 0.0f;
        return float(currentPos.y - lastPos.y);
    }

    void MouseInput::PostUpdate()
    {
        lastPos = currentPos;
    }

    const POINT& MouseInput::GetMousePos() const
    {
        POINT pos;
        GetCursorPos(&pos);
        ScreenToClient(GetConsoleWindow(), &pos);
        return pos;
    }

}