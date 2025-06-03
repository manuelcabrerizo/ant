#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Sphere;
class Cylinder;

class Segment
{
public:
    void Init(Vector3 a, Vector3 b);
    bool Intersect(Triangle& triangle, f32& t);
    bool Intersect(Sphere& sphere, f32& t);
    bool Intersect(Cylinder& cylinder, f32& t);
    Vector3 ClosestPoint(Vector3 point, f32& t);

    Vector3 a;
    Vector3 b;
};
