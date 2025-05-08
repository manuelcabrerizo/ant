#pragma once

class Vector3
{
public:
    union
    {
        struct
        {
            float x, y, z;
        };
        float v[3];
    };

    Vector3();
    Vector3(float value);
    Vector3(float x, float y, float z);
    float& operator[](int index);
    Vector3 operator+(const Vector3& rhs);
    void operator+=(const Vector3& rhs);
    Vector3 operator-(const Vector3& rhs);
    void operator-=(const Vector3& rhs);
    Vector3 operator*(float rhs);
    void operator*=(float rhs);
    float Dot(const Vector3& vector);
    Vector3 Cross(const Vector3& vector);
    float Magnitude();
    float MagnitudeSq();
    void Normalize();
    Vector3 Normalized();
    Vector3 Lerp(Vector3 vector, float t);
};