#pragma once
#include "Common/Common.h"
#include <vector>
#include <Windows.h>

namespace RenderEngine
{
    class NAOMI_API Texture
    {
    public:
        Texture(int width, int height)
            : width(width), height(height)
        {
            texture.assign(width * height, 0.0);
            finalBuffer.resize(width * height);
        };
        ~Texture() = default;

        void SetTexture(const std::vector<double>& texture);
        virtual void UpdateTexture();
        std::vector<CHAR_INFO> GetFinalBuffer();
        void SetAStarPos(int x, int y, int endX, int endY);

    protected:
        int width;
        int height;
        std::vector<double> texture;
        std::vector<CHAR_INFO> finalBuffer;
        int astarPosX = 0;
        int astarPosY = 0;
        int astarEndPosX = 0;
        int astarEndPosY = 0;
    };
}