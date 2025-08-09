#include "plane.h"
#include <math.h>

#include "triangle.h"
#include "ray.h"
#include "segment.h"

void Plane::Init(const Vector3& n, f32 d)
{
    this->n = n;
    this->d = d;
}

void Plane::Init(const Vector3& point, const Vector3 n)
{
    this->n = n;
    this->d = point.Dot(n);
}


void Plane::Init(const Triangle& triangle)
{
    n = triangle.n.Normalized();
    d = triangle.a.Dot(n);
}

bool Plane::Intersect(const Ray& ray, f32& t) const
{
    f32 denom = n.Dot(ray.GetDirection());
    if (fabsf(denom) > 0.0001f)
    {
        Vector3 center = n * d;
        t = (center - ray.GetOrigin()).Dot(n) / denom;
        if (t >= 0) return true;
    }
    return false;
}

bool Plane::Intersect(const Segment& segment, f32& t) const
{
    Vector3 ab = segment.GetB() - segment.GetA();
    Vector3 dir = ab.Normalized();
    f32 len = ab.Magnitude();

    f32 denom = n.Dot(dir);
    if (fabsf(denom) > 0.0001f)
    {
        Vector3 center = n * d;
        t = (center - segment.GetA()).Dot(n) / denom;
        if (t >= 0 && t <= len) return true;
    }
    return false;
}

bool Plane::Intersect(const Vector3& point) const
{
    return n.Dot(point) - d <= 0.0f;
}

Vector3 Plane::ClosestPoint(const Vector3& q) const
{
    f32 t = (n.Dot(q) - d) / n.Dot(n);
    return q - n * t;
}

float Plane::DistPoint(const Vector3& q) const
{
    return (n.Dot(q) - d) / n.Dot(n);
}

Vector3 Plane::GetNormal() const
{
    return n;
}

float Plane::GetDistance() const
{
    return d;
}
