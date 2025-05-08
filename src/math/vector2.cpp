#include "vector2.h"
#include "algebra.h"

Vector2::Vector2()
{
    this->x = 0;
    this->y = 0;
}

Vector2::Vector2(float value)
{
    this->x = value;
    this->y = value;
}

Vector2::Vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}

float& Vector2::operator[](int index)
{
    ASSERT(index > 0 && index < 2);
    return v[index];
}

Vector2 Vector2::operator+(const Vector2& rhs)
{
    Vector2 result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    return result;
}

void Vector2::operator+=(const Vector2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

Vector2 Vector2::operator-(const Vector2& rhs)
{
    Vector2 result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    return result;
}

void Vector2::operator-=(const Vector2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

Vector2 Vector2::operator*(float rhs)
{
    Vector2 result;
    result.x = x * rhs;
    result.y = y * rhs;
    return result;
}

void Vector2::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
}

float Vector2::Dot(const Vector2& vector)
{
    return x * vector.x + y * vector.y;
}

float Vector2::Magnitude()
{
    return sqrtf(x * x + y * y);

}

float Vector2::MagnitudeSq()
{
    return x * x + y * y;
}

void Vector2::Normalize()
{
    float magnitude = sqrtf(x * x + y * y);
    if(magnitude > FLT_EPSILON)
    {
        x /= magnitude;
        y /= magnitude;
    }
}

Vector2 Vector2::Normalized()
{
    Vector2 result = *this;
    float magnitude = sqrtf(x * x + y * y);
    if(magnitude > FLT_EPSILON)
    {
        result.x /= magnitude;
        result.y /= magnitude;
    }
    return result;
}

Vector2 Vector2::Lerp(Vector2 vector, float t)
{
    Vector2 result;
    result.x = lerp(x, vector.x, t);
    result.y = lerp(y, vector.y, t);
    return result;
}