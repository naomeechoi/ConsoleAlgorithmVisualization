#pragma once
#include <vector>
#include <Windows.h>
#include "World/Texture.h"

class SphereTexture : public RenderEngine::Texture
{
public:
    SphereTexture(int width, int height)
        : RenderEngine::Texture(width, height)
    {
    }
    ~SphereTexture() = default;
    virtual void UpdateTexture() override;
    void SetRotation(std::vector<float>* rot);

private:
    std::vector<float>* rot = nullptr;
    // 내부 유틸: normal → UV 변환
    void NormalToUV(float nx, float ny, float nz, float& u, float& v);
};