#pragma once
#include <vector>
#include <Windows.h>

class Cube
{
public:
    enum Face
    {
        FACE_X,
        FACE_NEG_X,
        FACE_Y,
        FACE_NEG_Y,
        FACE_Z,
        FACE_NEG_Z,
    };

public:

    Cube(int texWidth, int texHeight, float size);

    void SetTexture(const std::vector<double>& texture);

    void SetRotation(float dx, float dy);

    void Submit(std::vector<CHAR_INFO>& buffer, int screenWidth, int screenHeight,
        int astarX =-1, int astarY =-1, int endX =-1, int endY =-1);


    Face FaceUV(float px, float py, float pz, float& u, float& v);

private:

    int texWidth;
    int texHeight;

    float size;

    std::vector<double> texture;

    float rot[9];
};