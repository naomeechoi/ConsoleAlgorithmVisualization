#pragma once
#include <vector>
#include <Windows.h>
#include "World/Mesh.h"

class PlaneMesh : public RenderEngine::Mesh
{
public:
	PlaneMesh(int width, int height)
		: RenderEngine::Mesh(width, height)
	{
	};
    ~PlaneMesh() = default;

    virtual void UpdateTexture() override;
};