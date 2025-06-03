#include "triangle.h"
#include <float.h>

#include "ray.h"
#include "segment.h"
#include "plane.h"

void Triangle::Init(Vector3 a, Vector3 b, Vector3 c)
{
    this->a = a;
    this->b = b;
    this->c = c;
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    // test if the normal is correct
    n = ac.Cross(ab).Normalized();
}

bool Triangle::Intersect(Ray& ray, f32& t)
{
    return ray.Intersect(*this, t);
}

bool Triangle::Intersect(Segment& segment, f32& t)
{
    return segment.Intersect(*this, t);
}

bool Triangle::PointInside(Vector3 q)
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

Vector3 Triangle::ClosestPoint(Vector3 q)
{
    Plane plane;
    plane.Init(*this);

    Vector3 pointInPlane = plane.ClosestPoint(q);
    f32 u, v, w;
    if (PointInside(pointInPlane))
    {
        return pointInPlane;
    }

    Segment edges[3];
    edges[0].Init(a, b);
    edges[1].Init(b, c);
    edges[2].Init(c, a);

    Vector3 result;

    f32 lenSq = FLT_MAX;
    for (i32 i = 0; i < 3; ++i)
    {
        f32 currentT;
        Vector3 closest = edges[i].ClosestPoint(q, currentT);

        f32 currentLenSq = (closest - q).MagnitudeSq();
        if (currentLenSq < lenSq)
        {
            lenSq = currentLenSq;
            result = closest;
        }
    }

    return result;
}