#pragma once

#include "vector3.h"
#include "vector4.h"

class Matrix4 
{
public:
    union {
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        float v[16];
    };

    Matrix4()
         : m11(1), m12(0), m13(0), m14(0),
           m21(0), m22(1), m23(0), m24(0),
           m31(0), m32(0), m33(1), m34(0),
           m41(0), m42(0), m43(0), m44(1) {}

    Matrix4(float d)
         : m11(d), m12(0), m13(0), m14(0),
           m21(0), m22(d), m23(0), m24(0),
           m31(0), m32(0), m33(d), m34(0),
           m41(0), m42(0), m43(0), m44(d) {}

    Matrix4(float _m11, float _m12, float _m13, float _m14,
         float _m21, float _m22, float _m23, float _m24,
         float _m31, float _m32, float _m33, float _m34,
         float _m41, float _m42, float _m43, float _m44)
         : m11(_m11), m12(_m12), m13(_m13), m14(_m14),
           m21(_m21), m22(_m22), m23(_m23), m24(_m24),
           m31(_m31), m32(_m32), m33(_m33), m34(_m34),
           m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}

    float *operator[](int index);

    Matrix4 operator+(const Matrix4 &m) const;
    Matrix4 operator*(float val) const;
    Matrix4 operator*(const Matrix4 &m) const;
    Vector4 operator*(const Vector4 &vec) const;


    static Vector3 TransformPoint(const Matrix4& mat, const Vector3 &vec);
    static Vector3 TransformVector(const Matrix4& mat, const Vector3 &vec);
    
    static Matrix4 Frustum(float l, float r, float b, float t, float n, float f);
    static Matrix4 Perspective(float fov, float aspect, float znear, float zfar);
    static Matrix4 Ortho(float l, float r, float b, float t, float n, float f);
    static Matrix4 LookAt(const Vector3& position, const Vector3& target, const Vector3& up);
    static Matrix4 Translate(float x, float y, float z);
    static Matrix4 Translate(const Vector3& pos);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 Scale(float x, float y, float z);
    static Matrix4 RotateX(float angle);
    static Matrix4 RotateY(float angle);
    static Matrix4 RotateZ(float angle);
    static Matrix4 TransformFromBasis(const Vector3& o, const Vector3& r, const Vector3& u, const Vector3& f);
    // TODO: make them not static
    static Matrix4 Transposed(const Matrix4 &m);
    static float Determinant(const Matrix4 &m);
    static Matrix4 Adjugate(const Matrix4 &m);
    static Matrix4 Inverse(const Matrix4 &m);
};