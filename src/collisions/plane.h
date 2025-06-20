#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;

class Plane
{
public:

    void Init(const Vector3& n, f32 d);
    void Init(const Triangle& triangle);

    bool Intersect(const Ray& ray, f32& t) const;
    bool Intersect(const Segment& segment, f32& t) const;

    Vector3 ClosestPoint(const Vector3& q) const;
    float DistPoint(const Vector3& q) const;

    Vector3 n;
    f32 d;
};
