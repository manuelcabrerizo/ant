#include "plane.h"
#include <math.h>

#include "triangle.h"
#include "ray.h"
#include "segment.h"

void Plane::Init(Vector3 n, f32 d)
{
    this->n = n;
    this->d = d;
}

void Plane::Init(Triangle& triangle)
{
    n = triangle.n.Normalized();
    d = triangle.a.Dot(n);
}

bool Plane::Intersect(Ray& ray, f32& t)
{
    f32 denom = n.Dot(ray.d);
    if (fabsf(denom) > 0.0001f)
    {
        Vector3 center = n * d;
        t = (center - ray.o).Dot(n) / denom;
        if (t >= 0) return true;
    }
    return false;
}

bool Plane::Intersect(Segment& segment, f32& t)
{
    Vector3 ab = segment.b - segment.a;
    Vector3 dir = ab.Normalized();
    f32 len = ab.Magnitude();

    f32 denom = n.Dot(dir);
    if (fabsf(denom) > 0.0001f)
    {
        Vector3 center = n * d;
        t = (center - segment.a).Dot(n) / denom;
        if (t >= 0 && t <= len) return true;
    }
    return false;
}


Vector3 Plane::ClosestPoint(Vector3 q)
{
    f32 t = (n.Dot(q) - d) / n.Dot(n);
    return q - n * t;
}