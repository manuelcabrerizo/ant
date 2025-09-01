#pragma once

#include "vector3.h"
#include "matrix4.h"

class Quaternion 
{
public:
    union {
        struct
        {
            float w, x, y, z;
        };
        struct
        {
            float scalar;
            Vector3 vector;
        };
        float v[4];
    };

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    Quaternion(const Vector3& v) : w(1), x(v.x), y(v.y), z(v.z) {}
    
    float operator[](int index) const;
    Quaternion operator*(float f) const;
    Quaternion operator/(float f) const;
    Quaternion operator+(const Quaternion &q) const;
    Quaternion operator*(const Quaternion &q) const;
    Vector3 operator*(const Vector3 &v) const;

    void Normalize();
    Quaternion Normalized() const;
    Matrix4 ToMatrix4() const;
    
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
    static Quaternion AngleAxis(float angle, const Vector3 &axis);
    static Quaternion FromTo(const Vector3& from, const Vector3& to);
    static Quaternion LookRotation(const Vector3& direcion, const Vector3& up);
    static Quaternion Inverse(const Quaternion& q);
    static Quaternion FromBasis(const Vector3& right, const Vector3& up, const Vector3& forward);
};