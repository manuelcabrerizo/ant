#pragma once

#include "vector3.h"
#include "matrix4.h"

class Quaternion 
{
public:
    union {
        struct {
            float w, x, y, z;
        };
        struct {
            float scalar;
            Vector3 vector;
        };
        float v[4];
    };

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    Quaternion(Vector3 v) : w(1), x(v.x), y(v.y), z(v.z) {}
    
    float operator[](int index);
    Quaternion operator*(float f);
    Quaternion operator/(float f);
    Quaternion operator+(Quaternion &q);
    Quaternion operator*(Quaternion &q);
    Vector3 operator*(Vector3 &v);

    void Normalize();
    Quaternion Normalized();
    Matrix4 ToMatrix4();

    static Quaternion Slerp(Quaternion a, Quaternion b, float t);
    static Quaternion AngleAxis(float angle, Vector3 &axis);
};