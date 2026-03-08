#pragma once
#include <vector>
#include <Windows.h>

class Sphere
{
public:
    Sphere(int texWidth, int texHeight, float radius = 1.0f);

    void SetTexture(const std::vector<double>& texture);
    void SetRotation(float yaw, float pitch);
    void Submit(std::vector<CHAR_INFO>& buffer, int screenWidth, int screenHeight, const std::vector<int>& aiPos);

private:
    int texWidth;
    int texHeight;
    float radius;
    float rot[9] = { 1, 0, 0,  0, 1, 0,  0, 0, 1 };

    std::vector<double> texture; // Reaction Diffusion 텍스처

    // 내부 유틸: normal → UV 변환
    void NormalToUV(float nx, float ny, float nz, float& u, float& v);
};