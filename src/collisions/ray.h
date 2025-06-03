#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Sphere;

class Ray
{
public:
    void Init(Vector3 o, Vector3 d);
    bool Intersect(Triangle& triangle, f32& t);
    bool Intersect(Sphere& sphere, f32& t);

    Vector3 o;
    Vector3 d;
};