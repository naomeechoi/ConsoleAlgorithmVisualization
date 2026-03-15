#pragma once
#include <vector>
#include <Windows.h>
#include "World/Texture.h"

class PlaneTexture : public RenderEngine::Texture
{
public:
	PlaneTexture(int width, int height)
		: RenderEngine::Texture(width, height)
	{
	};
    ~PlaneTexture() = default;

    virtual void UpdateTexture() override;
};