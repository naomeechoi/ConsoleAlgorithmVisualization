#define NOMINMAX
#include "Cube.h"
#include <cmath>
#include <algorithm>
#include <vector>

Cube::Cube(int texWidth, int texHeight, float size)
    : texWidth(texWidth), texHeight(texHeight), size(size)
{
    texture.resize(texWidth * texHeight, 0.0);

    rot[0] = 1; rot[1] = 0; rot[2] = 0;
    rot[3] = 0; rot[4] = 1; rot[5] = 0;
    rot[6] = 0; rot[7] = 0; rot[8] = 1;
}

void Cube::SetTexture(const std::vector<double>& texture)
{
    this->texture = texture;
}

void Cube::SetRotation(float dx, float dy)
{
    float cosX = cos(dx), sinX = sin(dx);
    float cosY = cos(dy), sinY = sin(dy);

    float prev[9];
    std::copy(rot, rot + 9, prev);

    /* y축 기준 회전
    [cosθ, 0,  sinθ]
    [0,     1,   0   ]
    [−sinθ, 0, cosθ]
     */
    rot[0] = prev[0] * cosX - prev[2] * sinX;
    rot[1] = prev[1];
    rot[2] = prev[0] * sinX + prev[2] * cosX;

    rot[3] = prev[3] * cosX - prev[5] * sinX;
    rot[4] = prev[4];
    rot[5] = prev[3] * sinX + prev[5] * cosX;

    rot[6] = prev[6] * cosX - prev[8] * sinX;
    rot[7] = prev[7];
    rot[8] = prev[6] * sinX + prev[8] * cosX;

    // 누적
    std::copy(rot, rot + 9, prev);

    /* x축 기준 회전
    [1,  0,    0   ]
    [0,cosθ,−sinθ]
    [0,sinθ, cosθ]​
    */
    rot[0] = prev[0];
    rot[1] = prev[1] * cosY + prev[2] * sinY;
    rot[2] = prev[1] * sinY * -1.0f + prev[2] * cosY;

    rot[3] = prev[3];
    rot[4] = prev[4] * cosY + prev[5] * sinY;
    rot[5] = prev[4] * sinY * -1.0f + prev[5] * cosY;

    rot[6] = prev[6];
    rot[7] = prev[7] * cosY + prev[8] * sinY;
    rot[8] = prev[7] * sinY * -1.0f + prev[8] * cosY;

    // 행렬곱이 많아지면 부동소수점 오차가 생길 수 있어, 이를 막기 위해 정규화 한다.
    // x축 방향 벡터, y축 방향 벡터, z축 방향 백터 각각 정규화하기 위해 3번 룹을 돈다.
    for (int i = 0; i < 3; i++) {
        int idx = i * 3;
        float len = sqrt(rot[idx] * rot[idx]
            + rot[idx + 1] * rot[idx + 1]
            + rot[idx + 2] * rot[idx + 2]);

        rot[idx] /= len;
        rot[idx + 1] /= len;
        rot[idx + 2] /= len;
    }
}

Cube::Face Cube::FaceUV(float px, float py, float pz, float& u, float& v)
{
    float ax = fabs(px), ay = fabs(py), az = fabs(pz);

    // 레이가 정육면체에 닿았다는 것은
    // x축, 혹은 y축, z축 면에 닿았다는 뜻이다.
    // 각 축에 면은 두개가 있다.
    // 그리고 그 최소 최대값은 -size, size이다.
    // 만약 px가 -1이거나 1이면 x축 한 면에 닿았다는 뜻이다.
    // 그러나 부동소수점은 딱 떨어지는 정수값을 보장하지 않으므로
    // 다른 값보다 큰 상태인 비교한다.
    if (ax >= ay && ax >= az)
    { // X면
        u = (pz / size + 1.0f) * 0.5f;
        v = (py / size + 1.0f) * 0.5f;
        return (px > 0) ? FACE_X : FACE_NEG_X;
    }
    else if (ay >= ax && ay >= az) { // Y면
        u = (px / size + 1.0f) * 0.5f;
        v = (pz / size + 1.0f) * 0.5f;
        return (py > 0) ? FACE_Y : FACE_NEG_Y;
    }
    else { // Z면
        u = (px / size + 1.0f) * 0.5f;
        v = (py / size + 1.0f) * 0.5f;
        return (pz > 0) ? FACE_Z : FACE_NEG_Z;
    }
}

void Cube::Submit(std::vector<CHAR_INFO>& buffer, int screenWidth, int screenHeight)
{
    const float consoleCharAspect = 2.0f;

    float halfWidth = screenWidth / 2.0f;
    float halfHeight = screenHeight / 2.0f;

    float screenAspectRatio = (float)screenWidth / (float)screenHeight;

    float rayOriginZ = -2.7f;

    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            float nx = ((x - halfWidth) / halfWidth) * screenAspectRatio / consoleCharAspect;
            float ny = (y - halfHeight) / halfHeight;

            float dirX = nx;
            float dirY = ny;
            float dirZ = 1.0f;

            float ox = 0;
            float oy = 0;
            float oz = rayOriginZ;

            // 회전을 레이 방향과, 원점에 모두 적용한다.
            float rdirX = dirX * rot[0] + dirY * rot[1] + dirZ * rot[2];
            float rdirY = dirX * rot[3] + dirY * rot[4] + dirZ * rot[5];
            float rdirZ = dirX * rot[6] + dirY * rot[7] + dirZ * rot[8];
            
            float rox = ox * rot[0] + oy * rot[1] + oz * rot[2];
            float roy = ox * rot[3] + oy * rot[4] + oz * rot[5];
            float roz = ox * rot[6] + oy * rot[7] + oz * rot[8];

            dirX = rdirX;
            dirY = rdirY;
            dirZ = rdirZ;
            ox = rox;
            oy = roy;
            oz = roz;

            /*
            [-size, +size] 는 큐브의 범위
            레이 방정식: P(t) = O + tD
            O는 레이의 시작점이고, D는 레이의 방향, t를 레이가 앞으로 나가는 거리이다.
            -size ≤ P(t) ≤ +size 여야, 해당 지점이 특정 축 안에 있다고 말할 수 있다.
            -size ≤ O + tD ≤ +size
            (-size - O) / D ≤ t ≤ (size - O) / D
            세 축을 모두 검사해야한다.
            */
            std::vector dir = { dirX , dirY, dirZ };
            std::vector origin = { ox , oy, oz };
            bool isIntersect = true;
            float tMin = 0.0f, tMax = 1000.0f;

            for (int i = 0; i < 3; i++)
            {
                if (fabs(dir[i]) < 1e-6f)
                    continue;

                float minT = (-1 * size - origin[i]) / dir[i];
                float maxT = (size - origin[i]) / dir[i];
                if (minT > maxT)
                    std::swap(minT, maxT);

                // 공통 구간 없으면 충돌 없음
                if (tMax < minT || tMin > maxT) {
                    isIntersect = false;
                    break;
                }

                tMin = std::max(tMin, minT);
                tMax = std::min(tMax, maxT);
            }

            int idx = x + y * screenWidth;
            if (!isIntersect)
            {
                buffer[idx].Char.AsciiChar = ' ';
                buffer[idx].Attributes = 0;
                continue;
            }

            float px = ox + dirX * tMin;
            float py = oy + dirY * tMin;
            float pz = oz + dirZ * tMin;

            float u;
            float v;
            Face face = FaceUV(px, py, pz, u, v);

            std::vector<std::vector<int>> cubeOffsets = {
                {2, 1}, // FACE_X
                {0, 1}, // FACE_NEG_X
                {1, 0}, // FACE_Y
                {1, 2}, // FACE_NEG_Y
                {1, 1}, // FACE_Z
                {3, 1}  // FACE_NEG_Z
            };

            // 2. 스위치 문 대신 한 줄로 끝내기
            std::vector<int> cubeOffset = cubeOffsets[face];
            float texU = (u + cubeOffset[0]) / 4.0f;
            float texV = (v + cubeOffset[1]) / 3.0f;

            int tx = std::clamp(int(texU * texWidth), 0, texWidth - 1);
            int ty = std::clamp(int(texV * texHeight), 0, texHeight - 1);
            int texIdx = tx + ty * texWidth;
            double d = texture[texIdx];

            // 현재 점이 모서리인지 판단한다
            bool isEdge = false;
            float threshold = size * 0.05f;
            int edgeCount = 0;

            // 만약 어떤 면과의 거리가 매우 가깝다면(threshold 이하로)
            // 엣지 카운트를 올린다.
            // 이 엣지 카운트가 2개 이상이며
            // 2개의 면 이상과 아주 가깝다는 뜻이기 때문에 모서리에 가까운 부분이다.
            if (size - fabs(px) < threshold) edgeCount++;
            if (size - fabs(py) < threshold) edgeCount++;
            if (size - fabs(pz) < threshold) edgeCount++;
            if (edgeCount >= 2) isEdge = true;

            char c; int color;
            if (d > 0.35) { c = '&'; color = 15; }
            else if (d > 0.25) { c = '#'; color = 14; }
            else if (d > 0.15) { c = '*'; color = 11; }
            else if (d > 0.05) { c = '.'; color = 8; }
            else { c = ' '; color = 0; }

            if (isEdge) color = 1;

            buffer[idx].Char.AsciiChar = c;
            buffer[idx].Attributes = (WORD)color;
        }
    }
}