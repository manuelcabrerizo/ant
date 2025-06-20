#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Sphere;

class Ray
{
public:
    void Init(Vector3 o, Vector3 d);
    bool Intersect(const Triangle& triangle, f32& t) const;
    bool Intersect(const Sphere& sphere, f32& t) const;

    Vector3 o;
    Vector3 d;
};