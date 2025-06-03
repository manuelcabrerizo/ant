#pragma once

#include <common.h>
#include <math/vector3.h>

class Ray;
class Segment;

class Triangle
{
public:
    void Init(Vector3 a, Vector3 b, Vector3 c);
    bool Intersect(Ray& ray, f32& t);
    bool Intersect(Segment& segment, f32& t);

    bool PointInside(Vector3 q);
    Vector3 ClosestPoint(Vector3 q);

    Vector3 a, b, c;
    Vector3  n;
};