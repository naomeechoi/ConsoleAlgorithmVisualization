#pragma once
#include "Common/Common.h"
#include <vector>
#include <Windows.h>

namespace RenderEngine
{
    class NAOMI_API Mesh
    {
    public:
        Mesh(int width, int height)
            : width(width), height(height)
        {
            texture.assign(width * height, 0.0);
            finalBuffer.resize(width * height);
        };
        ~Mesh() = default;

        void SetTexture(const std::vector<double>& texture);
        virtual void UpdateTexture();
        std::vector<CHAR_INFO> GetFinalBuffer();

    protected:
        int width;
        int height;
        std::vector<double> texture;
        std::vector<CHAR_INFO> finalBuffer;
    };
}