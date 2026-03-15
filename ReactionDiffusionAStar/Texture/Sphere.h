#pragma once
#include <vector>
#include <Windows.h>

class Sphere
{
public:
    Sphere(int texWidth, int texHeight, float radius = 1.0f);

    void SetTexture(const std::vector<double>& texture);
    void SetRotation(float yaw, float pitch);
    void Submit(std::vector<CHAR_INFO>& buffer, int screenWidth, int screenHeight);
    const float GetRadius() const { return radius; };
    const std::vector<float>& GetRotation() const { return rot; };

private:
    int texWidth;
    int texHeight;
    float radius;
    std::vector<float> rot = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    std::vector<double> texture; // Reaction Diffusion 텍스처

    // 내부 유틸: normal → UV 변환
    void NormalToUV(float nx, float ny, float nz, float& u, float& v);
};