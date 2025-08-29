#include "utils.h"

Assimp::Importer Utils::importer;

Matrix4 Utils::FromAssimp(aiMatrix4x4 m)
{
    Matrix4 mat = Matrix4(m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
    return Matrix4::Transposed(mat);
}

Vector3 Utils::FromAssimp(aiVector3D m)
{
    return Vector3(m.x, m.y, m.z);
}

Quaternion Utils::FromAssimp(aiQuaternion m)
{
    return Quaternion(m.w, m.x, m.y, m.z);
}

i32 Utils::NextPower2(u32  x)
{
    int  value = 1;
    while (value <= x)
    {
        value = value << 1;
    }
    return  value;
}

float Utils::Clamp(float n, float min, float max)
{
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

void Utils::Swap(float& a, float& b)
{
    float temp = a;
    a = b;
    b = temp;
}

float Utils::RandRange(float min, float max)
{
    float t = rand() / (float)RAND_MAX;
    return (1.0f - t) * min + t * max;
}