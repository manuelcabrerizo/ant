#include "vector3.h"
#include "algebra.h"

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
    ASSERT(index > 0 && index < 3);
    return v[index];
}

Vector3 Vector3::operator+(const Vector3& rhs)
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

Vector3 Vector3::operator-(const Vector3& rhs)
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

Vector3 Vector3::operator*(float rhs)
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

float Vector3::Dot(const Vector3& vector)
{
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3 Vector3::Cross(const Vector3& vector)
{
    return Vector3(
        y * vector.z - z * vector.y, 
        z * vector.x - x * vector.z, 
        x * vector.y - y * vector.x 
    );
}

float Vector3::Magnitude()
{
    return sqrtf(x * x + y * y + z * z);
}

float Vector3::MagnitudeSq()
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

Vector3 Vector3::Normalized()
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

Vector3 Vector3::Lerp(Vector3 vector, float t) 
{
    Vector3 result;
    result.x = lerp(x, vector.x, t);
    result.y = lerp(y, vector.y, t);
    result.z = lerp(z, vector.z, t);
    return result;
}