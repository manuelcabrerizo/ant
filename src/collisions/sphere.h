#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;
class Plane;

class Sphere
{
public:
    void Init(Vector3 c, f32 r);

    bool Intersect(Ray& ray, f32& t);
    bool Intersect(Segment& segment, f32& t);
    bool Intersect(Triangle& triangle, Vector3& n, f32& penetration);

    bool DynamicIntersect(Plane& plane, Vector3 movement, f32& t);
    bool DynamicIntersect(Triangle& triangle, Vector3 movement, f32& u, f32& v, f32& w,
        f32& t, Vector3& n);

    Vector3 c;
    f32 r;
};