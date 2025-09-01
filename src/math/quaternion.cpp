#include "quaternion.h"
#include <common.h>
#include <math.h>
#include <float.h>

float Quaternion::operator[](int index) const
{
    ASSERT(index >= 0 && index < 4);
    return v[index];
}

Quaternion Quaternion::operator*(float f) const
{
    Quaternion result; 
    result.w = this->w * f;
    result.x = this->x * f;
    result.y = this->y * f;
    result.z = this->z * f;
    return result;
}

Quaternion Quaternion::operator/(float f) const
{
    Quaternion result = *this * (1.0f/f); 
    return result;
}

Quaternion Quaternion::operator+(const Quaternion &q) const
{
    Quaternion result; 
    result.w = this->w + q.w;
    result.x = this->x + q.x;
    result.y = this->y + q.y;
    result.z = this->z + q.z;
    return result;
}

Quaternion Quaternion::operator*(const Quaternion &q) const
{
	return Quaternion(
		-q.x * x - q.y * y - q.z * z + q.w * w,
         q.x * w + q.y * z - q.z * y + q.w * x,
		-q.x * z + q.y * w + q.z * x + q.w * y,
	     q.x * y - q.y * x + q.z * w + q.w * z
	);
}

Vector3 Quaternion::operator*(const Vector3 &v) const
{
    return vector * 2.0f * vector.Dot(v) +
        v * (scalar * scalar - vector.Dot(vector)) +
        vector.Cross(v) * 2.0f * scalar;
}

void Quaternion::Normalize()
{
    float magnitudeSqr = w*w + x*x + y*y + z*z;
    if(magnitudeSqr < FLT_EPSILON) {
        return;
    }

    float magnitude = sqrtf(magnitudeSqr);
    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

Quaternion Quaternion::Normalized() const
{
    float magnitudeSqr = w*w + x*x + y*y + z*z;
    if(magnitudeSqr < FLT_EPSILON) {
        return {};
    }
    else
    {
        float magnitude = sqrtf(magnitudeSqr);
        Quaternion result;
        result.w = w / magnitude;
        result.x = x / magnitude;
        result.y = y / magnitude;
        result.z = z / magnitude;
        return result;
    }
}

Matrix4 Quaternion::ToMatrix4() const
{
    Matrix4 result;

    result.v[0] = 1 - 2*y*y - 2*z*z;
    result.v[1] = 2 * (x*y - w*z);
    result.v[2] = 2 * (x*z + w*y);
    result.v[3] = 0;

    result.v[4] = 2 * (x*y + w*z);
    result.v[5] = 1 - 2*x*x - 2*z*z;
    result.v[6] = 2 * (y*z - w*x);
    result.v[7] = 0;

    result.v[8]  = 2 * (x*z - w*y);
    result.v[9]  = 2 * (y*z + w*x);
    result.v[10] = 1 - 2*x*x - 2*y*y;
    result.v[11] = 0;

    result.v[12] = 0;
    result.v[13] = 0;
    result.v[14] = 0;
    result.v[15] = 1;

    return Matrix4::Transposed( result );
}


Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b_, float t)
{
    Quaternion b = b_;

    float k0, k1;
    float cos_omega = a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
    if(cos_omega < 0.0f) {
        b.w = -b.w;
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        cos_omega = -cos_omega;
    }
    
    if(cos_omega > 0.9999f) {
        k0 = (1 - t);
        k1 = t;
    } else {
        
        float sin_omega = sqrtf(1.0f - cos_omega*cos_omega);
        float omega = atan2(sin_omega, cos_omega);
        float one_over_sin_omega = 1.0f / sin_omega;
        k0 = sin((1.0f - t) * omega) * one_over_sin_omega;
        k1 = sin(t * omega) * one_over_sin_omega;
    }

    Quaternion result;

    result.w = a.w*k0 + b.w*k1;
    result.x = a.x*k0 + b.x*k1;
    result.y = a.y*k0 + b.y*k1;
    result.z = a.z*k0 + b.z*k1;

    return result;
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3 &axis)
{
    Vector3 norm = axis.Normalized();
    float s = sinf(angle * 0.5f);
    return  Quaternion(cosf(angle * 0.5f),
                        norm.x * s,
                        norm.y * s,
                        norm.z * s);
}

Quaternion Quaternion::FromTo(const Vector3& from, const Vector3& to)
{
    Vector3 f = from.Normalized();
    Vector3 t = to.Normalized();
    // make sure they are not the same vector
    float dot = Vector3::Dot(f, t);
    if (dot > 0.9999f) 
    {
        return Quaternion();
    }
    // check whether the two vectors are oposites of each other.
    // if they are the most orthogonal axis of the from can be used
    // to create a pure quaternion
    else if (dot < -0.9999f)
    {
        Vector3 ortho = Vector3(1.0f, 0.0f, 0.0f);
        if (fabsf(f.y) < fabsf(f.x))
        {
            ortho = Vector3(0.0f, 1.0f, 0.0f);
        }
        if (fabsf(f.z) < fabsf(f.y) && fabsf(f.z) < fabsf(f.x)) 
        {
            ortho = Vector3(0.0f, 0.0f, 1.0f);
        }
        Vector3 axis = Vector3::Cross(f, ortho).Normalized();
        return Quaternion(axis.x, axis.y, axis.z, 0.0f);
    }
    // create a half vector between the from and to vectors
    // use the cross product of the half vector and the starting
    // vector to calculate the axis of rotation and the dot product
    // of the two to find the angle of rotation
    Vector3 half = (f + t).Normalized();
    Vector3 axis = Vector3::Cross(f, half);
    return Quaternion(axis.x, axis.y, axis.z, Vector3::Dot(f, half));
}

Quaternion Quaternion::LookRotation(const Vector3& direcion, const Vector3& up)
{
    // Find orthonormal basis vectors
    Vector3 f = direcion.Normalized();
    Vector3 u = up.Normalized();
    Vector3 r = Vector3::Cross(u, f);
    u = Vector3::Cross(f, r);
    // From world forward to object forward
    Quaternion f2d = FromTo(Vector3(0, 0, 1), f);
    // what direction is the new object up?
    Vector3 objectUp = f2d * Vector3(0, 1, 0);
    // From object up to desired up
    Quaternion u2u = FromTo(objectUp, u);
    // Rotate to forward direction first, then twist to correct up
    Quaternion result = f2d * u2u;
    // Don’t forget to normalize the result
    return result.Normalized();
}

Quaternion Quaternion::FromBasis(const Vector3& right, const Vector3& up, const Vector3& forward)
{
    // Rotation matrix transposed
    float m00 = right.x, m01 = up.x, m02 = forward.x;
    float m10 = right.y, m11 = up.y, m12 = forward.y;
    float m20 = right.z, m21 = up.z, m22 = forward.z;

    float trace = m00 + m11 + m22;
    Quaternion q;

    if (trace > 0.0f) {
        float s = sqrt(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (m21 - m12) / s;
        q.y = (m02 - m20) / s;
        q.z = (m10 - m01) / s;
    }
    else if ((m00 > m11) && (m00 > m22)) {
        float s = sqrt(1.0f + m00 - m11 - m22) * 2.0f;
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    }
    else if (m11 > m22) {
        float s = sqrt(1.0f + m11 - m00 - m22) * 2.0f;
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    }
    else {
        float s = sqrt(1.0f + m22 - m00 - m11) * 2.0f;
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    return q;
}

#define QUAT_EPSILON 0.000001f

Quaternion Quaternion::Inverse(const Quaternion& q)
{
    float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (lenSq < QUAT_EPSILON) {
        return Quaternion();
    }
    float recip = 1.0f / lenSq;
    return Quaternion(-q.x * recip, -q.y * recip, -q.z * recip, q.w * recip);
}
