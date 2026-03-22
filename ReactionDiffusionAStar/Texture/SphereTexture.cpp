#define NOMINMAX
#include "SphereTexture.h"
#include <cmath>
#include <algorithm>

const float PI = 3.14159265f;
const float radius = 1.0f;

void SphereTexture::UpdateTexture()
{
    const float consoleCharAspect = 2.0f;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float screenAspectRatio = (float)width / (float)height;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // 구의 중심을 중앙으로 맞춘 후 좌표를 정규화 한다.
            // 실제 스크린 비율, 예를 들어 width가 140, height이 80이라면
            // width가 더 길기 때문에 이 비율을 맞춰주기 위해 screenAspectRatio를 곱해준다.
            // 콘솔에서는 Y축이 X축보다 2배 더 길게 표현되기 때문에
            // X축을 1/2배 해준다.(압축)
            float nx = ((x - halfWidth) / halfWidth) * screenAspectRatio / consoleCharAspect;
            float ny = (y - halfHeight) / halfHeight;
            float length = nx * nx + ny * ny;

            // x,y 원점으로 부터 좌표가 떨어져 있는 길이
            // 가 반지름 범위를 벗어나면 그리지 않는다.
            int idx = x + y * width;
            if (length > radius * radius) {
                finalBuffer[idx].Char.AsciiChar = ' ';
                finalBuffer[idx].Attributes = 0;
                continue;
            }

            // 구의 방정식을 이용해 z 좌표를 구한다.
            // x^2 + y^2 + z^2 = R^2
            // z^2 = R^2 - x^2 - y^2 
            float nz = sqrt(radius * radius - length);

            // 회전된 좌표를 구한다.
            float nxAfterRot = nx * (*rot)[0] + ny * (*rot)[1] + nz * (*rot)[2];
            float nyAfterRot = nx * (*rot)[3] + ny * (*rot)[4] + nz * (*rot)[5];
            float nzAfterRot = nx * (*rot)[6] + ny * (*rot)[7] + nz * (*rot)[8];

            float u = 0;
            float v = 0;
            NormalToUV(nxAfterRot, nyAfterRot, nzAfterRot, u, v);

            // 구해온 u, v 즉 0~1 범위로 정규화된 점에 다시 width, height를 곱해주면
            // width, height 범위로 확대된다.
            // 이 범위가 범위를 벗어나지 않도록 클램프 해준다.
            int tx = std::clamp(int(u * width), 0, width - 1);
            int ty = std::clamp(int(v * height), 0, height - 1);

            // 텍스쳐의 농도를 가져온다.
            int idxInTexture = tx + ty * width;
            double d = texture[idxInTexture];

            char c; int color;

            if (d > 0.35) { c = '&'; color = 15; }
            else if (d > 0.25) { c = '#'; color = 14; }
            else if (d > 0.15) { c = '*'; color = 11; }
            else if (d > 0.05) { c = '.'; color = 8; }
            else { c = ' '; color = 0; }

            finalBuffer[idx].Char.AsciiChar = c;
            finalBuffer[idx].Attributes = (WORD)color;
        }
    }
}

void SphereTexture::SetRotation(std::vector<float>* rot)
{
    this->rot = rot;
}

void SphereTexture::NormalToUV(float nx, float ny, float nz, float& u, float& v)
{
    // nx, ny, nz는 구의 원점에서 밖으로 향하는 방향 벡터이다.
    // 이 방향 벡터와 원점 간의 각도를 통해
    // 텍스쳐를 가로 0~1, 세로 0~1로 비율로 줄였을 때
    // 현재 nx, ny, nz가 텍스처의 어떤 위치에 있는지 구할 수 있다.

    // 텍스처 가로에서 위치 찾아내기
    // x축과 z축 사이의 각도를 atan2로 알아내서
    // atan2의 범위는 -PI ~ PI 이다.
    // 이 범위를 0~1의 범우로 정규화한다.
    u = 0.5f + atan2(nz, nx) / (2.0f * PI);

    // 텍스처 세로에서 위치 찾아내기
    // asin(ny)를 통해 y각도를 알아내
    // asin의 범위는 -PI/2 ~ PI/2이다. 
    // 이 범위를 0~1의 범위로 정규화 한다.
    v = 0.5f - asin(ny) / PI;
    // Linear 비율 맵핑은 문제 없는데 각도로 하는게 구의 형태에 더 잘 맞출 수 있음
    //v = 1.0f - (ny + 1)/2;
}