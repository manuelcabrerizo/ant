#include "triangle.h"
#include <float.h>

#include "ray.h"
#include "segment.h"
#include "plane.h"
#include <graphics_manager.h>

void Triangle::Init(const Vector3& a, const Vector3& b, const Vector3& c)
{
    this->a = a;
    this->b = b;
    this->c = c;
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    // test if the normal is correct
    n = ac.Cross(ab).Normalized();
}

bool Triangle::Intersect(const Ray& ray, float& t) const
{
    return ray.Intersect(*this, t);
}

bool Triangle::Intersect(const Segment& segment, float& t) const
{
    return segment.Intersect(*this, t);
}

bool Triangle::PointInside(const Vector3& q) const
{
    Vector3 v0 = b - a;
    Vector3 v1 = c - a;
    Vector3 v2 = q - a;

    float d00 = v0.Dot(v0);
    float d10 = v1.Dot(v0);
    float d11 = v1.Dot(v1);
    float d20 = v2.Dot(v0);
    float d21 = v2.Dot(v1);

    float invDenom = 1.0f / ((d00 * d11) - (d10 * d10));

    float y = ((d20 * d11) - (d10 * d21)) * invDenom;
    float z = ((d00 * d21) - (d20 * d10)) * invDenom;
    float x = 1.0f - y - z;

    return x >= 0.0f && x <= 1.0f &&
        y >= 0.0f && y <= 1.0f &&
        z >= 0.0f && z <= 1.0f;
}


// TODO: test others implementation
Vector3 Triangle::ClosestPoint(const Vector3& q) const
{
    Plane plane;
    plane.Init(*this);

    Vector3 pointInPlane = plane.ClosestPoint(q);
    if (PointInside(pointInPlane))
    {
        return pointInPlane;
    }

    Segment edges[3];
    edges[0].Init(a, b);
    edges[1].Init(b, c);
    edges[2].Init(c, a);

    Vector3 result;

    float lenSq = FLT_MAX;
    for (i32 i = 0; i < 3; ++i)
    {
        float currentT;
        Vector3 closest = edges[i].ClosestPoint(q, currentT);

        float currentLenSq = (closest - q).MagnitudeSq();
        if (currentLenSq < lenSq)
        {
            lenSq = currentLenSq;
            result = closest;
        }
    }

    return result;
}

void Triangle::DebugDraw(const Vector3& color)
{
    GraphicsManager::Get()->DebugDrawLine(a, b, color);
    GraphicsManager::Get()->DebugDrawLine(b, c, color);
    GraphicsManager::Get()->DebugDrawLine(c, a, color);
}