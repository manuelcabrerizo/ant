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
    float operator[](int index) const;
    Vector2 operator+(const Vector2& rhs) const;
    void operator+=(const Vector2& rhs);
    Vector2 operator-(const Vector2& rhs) const;
    void operator-=(const Vector2& rhs);
    Vector2 operator*(float rhs) const;
    void operator*=(float rhs);
    float Dot(const Vector2& vector) const;
    float Magnitude() const;
    float MagnitudeSq() const;
    void Normalize();
    Vector2 Normalized() const;
    Vector2 Lerp(const Vector2& vector, float t) const;
};