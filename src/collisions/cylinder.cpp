#include "cylinder.h"
#include <graphics_manager.h>
#include <math/algebra.h>
#include <math/quaternion.h>


void Cylinder::Init(Vector3 p, Vector3 q, f32 r)
{
    this->p = p;
    this->q = q;
    this->r = r;
}

Vector3 Cylinder::GetP() const
{
    return p;
}

Vector3 Cylinder::GetQ() const
{
    return q;
}

float Cylinder::GetRadio() const
{
    return r;
}

void Cylinder::DebugDraw(int subdivision, const Vector3& color)
{
    float inc = (2.0f * ANT_PI) / (float)subdivision;

    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 axis = (q - p).Normalized();
    if(fabsf(axis.Dot(up)) > 0.5f)
    {
        up = Vector3(1.0f, 0.0f, 0.0f);
    }

    Vector3 current = axis.Cross(up).Normalized();
    for (i32 i = 0; i < subdivision; ++i)
    {
        Vector3 a = current * r;
        current = Quaternion::AngleAxis(inc, axis) * current;
        Vector3 b = current * r;
        GraphicsManager::Get()->DebugDrawLine(p + a, p + b, color);
        GraphicsManager::Get()->DebugDrawLine(q + a, q + b, color);
        GraphicsManager::Get()->DebugDrawLine(p + a, q + a, color);
        GraphicsManager::Get()->DebugDrawLine(p + b, q + b, color);
    }
}