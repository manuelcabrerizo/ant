#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;

class Plane
{
public:

    void Init(Vector3 n, f32 d);
    void Init(Triangle& triangle);

    bool Intersect(Ray& ray, f32& t);
    bool Intersect(Segment& segment, f32& t);

    Vector3 ClosestPoint(Vector3 q);

    Vector3 n;
    f32 d;
};
