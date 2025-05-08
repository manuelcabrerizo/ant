#pragma once

class Vector2
{
public:
    union
    {
        struct
        {
            float x, y;
        };
        float v[2];
    };

    Vector2();
    Vector2(float value);
    Vector2(float x, float y);
    float& operator[](int index);
    Vector2 operator+(const Vector2& rhs);
    void operator+=(const Vector2& rhs);
    Vector2 operator-(const Vector2& rhs);
    void operator-=(const Vector2& rhs);
    Vector2 operator*(float rhs);
    void operator*=(float rhs);
    float Dot(const Vector2& vector);
    float Magnitude();
    float MagnitudeSq();
    void Normalize();
    Vector2 Normalized();
    Vector2 Lerp(Vector2 vector, float t);
};