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
    float operator[](int index) const;
    Vector3 operator+(const Vector3& rhs) const;
    void operator+=(const Vector3& rhs);
    Vector3 operator-(const Vector3& rhs) const;
    void operator-=(const Vector3& rhs);
    Vector3 operator*(float rhs) const;
    void operator*=(float rhs);
    Vector3 operator/(float rhs) const;
    void operator/=(float rhs);
    float Dot(const Vector3& vector) const;
    Vector3 Cross(const Vector3& vector) const;
    float Magnitude() const;
    float MagnitudeSq() const;
    void Normalize();
    Vector3 Normalized() const;
    Vector3 Lerp(const Vector3& vector, float t) const;

    static float Dot(const Vector3& a, const Vector3& b);
    static Vector3 Cross(const Vector3& a, const Vector3& b);

};