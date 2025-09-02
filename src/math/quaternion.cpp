#include "quaternion.h"
#include <common.h>
#include <math.h>
#include <float.h>

#define QUAT_EPSILON 0.000001f

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
        w*q.w - x*q.x - y*q.y - z*q.z,
        w*q.x + x*q.w + y*q.z - z*q.y,
        w*q.y + y*q.w + z*q.x - x*q.z,
        w*q.z + z*q.w + x*q.y - y*q.x
	);
}

// TODO: test his function mabye we dont need it
Vector3 Quaternion::operator*(const Vector3 &v) const
{
    Quaternion p = Quaternion(0, v.x, v.y, v.z);
    p = *this * p * Inverse(*this);
    return Vector3(p.x, p.y, p.z);
}

float Quaternion::Magnitude() const
{
    float magnitudeSq = w * w + x * x + y * y + z * z;
    return sqrtf(magnitudeSq);
}

float Quaternion::MagnitudeSq() const
{
    float magnitudeSq = w * w + x * x + y * y + z * z;
    return magnitudeSq;
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


Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    float w0 = a.w, x0 = a.x, y0 = a.y, z0 = a.z;
    float w1 = b.w, x1 = b.x, y1 = b.y, z1 = b.z;

    // Compute the cosine of the angle betwiin the quaternions
    // using the dot product
    float cosOmega = w0 * w1 + x0 * x1 + y0 * y1 + z0 * z1;

    // If negative dot, negate one of the input 
    // quaternions, to take the shorter 4D arc
    if (cosOmega < 0.0f)
    {
        w1 = -w1;
        x1 = -x1;
        y1 = -y1;
        z1 = -z1;
        cosOmega = -cosOmega;
    }

    // Check if they are very close together, to protect
    // against divide by zero
    float k0, k1;
    if (cosOmega > 0.9999f)
    {
        // Very close, just use linear interpolation
        k0 = 1.0f - t;
        k1 = t;
    }
    else
    {
        // Compute the sin of the angle using the
        // trig identity sin^2(omega) + cos^2(omega) = 1
        float sinOmega = sqrtf(1.0f - cosOmega * cosOmega);
        // Compute the angle from its sine and cosine
        float omega = atan2f(sinOmega, cosOmega);
        // Compute inverse of denominator, so we only
        // have to divide once
        float oneOverSinOmega = 1.0f / sinOmega;
        // Compute the interpolation parameters
        k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
        k1 = sinf(t * omega) * oneOverSinOmega;
    }

    // Interpolate
    return Quaternion(
        w0*k0 + w1*k1,
        x0*k0 + x1*k1,
        y0*k0 + y1*k1,
        z0*k0 + z1*k1
    );
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3 &axis)
{
    Vector3 norm = axis.Normalized();
    float s = sinf(angle * 0.5f);
    return  Quaternion(cosf(angle * 0.5f), norm.x * s, norm.y * s, norm.z * s);
}

Quaternion Quaternion::FromMatrix(const Matrix4& m)
{
    float m11 = m.m11, m12 = m.m21, m13 = m.m31;
    float m21 = m.m12, m22 = m.m22, m23 = m.m32;
    float m31 = m.m13, m32 = m.m23, m33 = m.m33;

    // Determine whitch of w, x, y, or z has the largets absolute value
    float fourWSquaredMinus1 = m11 + m22 + m33;
    float fourXSquaredMinus1 = m11 - m22 - m33;
    float fourYSquaredMinus1 = m22 - m11 - m33;
    float fourZSquaredMinus1 = m33 - m11 - m22;

    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
    {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }

    // Perform Square root and division
    float biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    float mult = 0.25f / biggestVal;

    Quaternion q;

    // Apply table to compute quaternion values
    switch (biggestIndex)
    {
    case 0:
        q.w = biggestVal;
        q.x = (m23 - m32) * mult;
        q.y = (m31 - m13) * mult;
        q.z = (m12 - m21) * mult;
        break;
    case 1:
        q.x = biggestVal;
        q.w = (m23 - m32) * mult;
        q.y = (m12 - m21) * mult;
        q.z = (m31 - m13) * mult;
        break;
    case 2:        
        q.y = biggestVal;
        q.w = (m31 - m13) * mult;
        q.x = (m12 - m21) * mult;
        q.z = (m23 - m32) * mult;
        break;
    case 3:
        q.z = biggestVal;
        q.w = (m12 - m21) * mult;
        q.x = (m31 - m13) * mult;
        q.y = (m23 - m32) * mult;
        break;
    }

    return q;
}

Quaternion Quaternion::FromEuler(float x, float y, float z)
{
    // TODO: precompute the sin and cos to reduce function calls
    float hh = x/2.0f, hp = y/2.0f, hb = z/2.0f;
    return Quaternion(
        cosf(hh)*cosf(hp)*cosf(hb) + sinf(hh)*sinf(hp)*sinf(hb),
        cosf(hh)*sinf(hp)*cosf(hb) + sinf(hh)*cosf(hp)*sinf(hb),
        sinf(hh)*cosf(hp)*cosf(hb) - cosf(hh)*sinf(hp)*sinf(hb),
        cosf(hh)*cosf(hp)*sinf(hb) - sinf(hh)*sinf(hp)*cosf(hb)
    );
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
    // in quaternions that represent rotations the conjugate is
    // the inverse quaternion, so the division by the magnitude can be omited
    float magnitude = q.Magnitude();
    if (magnitude < QUAT_EPSILON) {
        return Quaternion();
    }
    float recip = 1.0f / magnitude;
    return Quaternion(q.w * recip , -q.x * recip, -q.y * recip, -q.z * recip);
}
