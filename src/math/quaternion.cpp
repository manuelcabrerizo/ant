#include "quaternion.h"
#include <common.h>
#include <math.h>
#include <float.h>

float Quaternion::operator[](int index)
{
    ASSERT(index > 0 && index < 4);
    return v[index];
}

Quaternion Quaternion::operator*(float f)
{
    Quaternion result; 
    result.w = this->w * f;
    result.x = this->x * f;
    result.y = this->y * f;
    result.z = this->z * f;
    return result;
}

Quaternion Quaternion::operator/(float f)
{
    Quaternion result = *this * (1.0f/f); 
    return result;
}

Quaternion Quaternion::operator+(Quaternion &q)
{
    Quaternion result; 
    result.w = this->w + q.w;
    result.x = this->x + q.x;
    result.y = this->y + q.y;
    result.z = this->z + q.z;
    return result;
}

Quaternion Quaternion::operator*(Quaternion &q)
{
	return Quaternion(
		-q.x * x - q.y * y - q.z * z + q.w * w,
         q.x * w + q.y * z - q.z * y + q.w * x,
		-q.x * z + q.y * w + q.z * x + q.w * y,
	     q.x * y - q.y * x + q.z * w + q.w * z
	);
}

Vector3 Quaternion::operator*(Vector3 &v)
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

Quaternion Quaternion::Normalized()
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

Matrix4 Quaternion::ToMatrix4()
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

    return result;
}

Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t)
{
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

Quaternion Quaternion::AngleAxis(float angle, Vector3 &axis)
{
    Vector3 norm = axis.Normalized();
    float s = sinf(angle * 0.5f);
    return  Quaternion(cosf(angle * 0.5f),
                        norm.x * s,
                        norm.y * s,
                        norm.z * s);
}