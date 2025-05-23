#include "vector4.h"

Vector4::Vector4()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

Vector4::Vector4(Vector3 vector, float scalar)
{
    this->x = vector.x;
    this->y = vector.y;
    this->z = vector.z;
    this->w = scalar;
}