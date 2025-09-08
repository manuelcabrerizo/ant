#include "vector3.h"
#include "algebra.h"
#include <common.h>
#include <float.h>

Vector3 Vector3::zero = Vector3(0, 0, 0);
Vector3 Vector3::right = Vector3(1, 0, 0);
Vector3 Vector3::up = Vector3(0, 1, 0);
Vector3 Vector3::forward = Vector3(0, 0, 1);
Vector3 Vector3::one = Vector3(1, 1, 1);

Vector3::Vector3()
{
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

Vector3::Vector3(float value)
{
    this->x = value;
    this->y = value;
    this->z = value;
}

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

float& Vector3::operator[](int index)
{
    ASSERT(index >= 0 && index < 3);
    if (index < 3)
    {
        return v[index];
    }
}

float Vector3::operator[](int index) const
{
    ASSERT(index >= 0 && index < 3);
    if (index < 3)
    {
        return v[index];
    }
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    Vector3 result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;
    return result;
}

void Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    Vector3 result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;
    return result;
}

void Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

Vector3 Vector3::operator*(float rhs) const
{
    Vector3 result;
    result.x = x * rhs;
    result.y = y * rhs;
    result.z = z * rhs;
    return result;
}

void Vector3::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
}

Vector3 Vector3::operator/(float rhs) const
{
    Vector3 result;
    result.x = x / rhs;
    result.y = y / rhs;
    result.z = z / rhs;
    return result;
}

void Vector3::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
}

float Vector3::Dot(const Vector3& vector) const
{
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3 Vector3::Cross(const Vector3& vector) const
{
    return Vector3(
        y * vector.z - z * vector.y, 
        z * vector.x - x * vector.z, 
        x * vector.y - y * vector.x 
    );
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

float Vector3::Magnitude() const
{
    return sqrtf(x * x + y * y + z * z);
}

float Vector3::MagnitudeSq() const
{
    return x * x + y * y + z * z;
}

void Vector3::Normalize()
{
    float magnitude = sqrtf(x * x + y * y + z * z);
    if(magnitude > FLT_EPSILON)
    {
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }
}

Vector3 Vector3::Normalized() const
{
    Vector3 result = *this;
    float magnitude = sqrtf(x * x + y * y + z * z);
    if(magnitude > FLT_EPSILON)
    {
        result.x /= magnitude;
        result.y /= magnitude;
        result.z /= magnitude;
    }
    return result;
}

Vector3 Vector3::Lerp(const Vector3& vector, float t) const
{
    Vector3 result;
    result.x = lerp(x, vector.x, t);
    result.y = lerp(y, vector.y, t);
    result.z = lerp(z, vector.z, t);
    return result;
}

float Vector3::Dot(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}