#pragma once
#include <vector>
#include <Windows.h>

class Plane
{
public:
    Plane(int width, int height)
        : width(width), height(height)
    {
    };
    ~Plane() = default;

    void SetTexture(const std::vector<double>& texture);
    void Submit(std::vector<CHAR_INFO>& buffer);

private:
    int width;
    int height;
    std::vector<double> texture;
};