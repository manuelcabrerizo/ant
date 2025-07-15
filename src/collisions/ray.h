#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Sphere;
class Capsule;
class Cylinder;

class Ray
{
public:
    void Init(Vector3 o, Vector3 d);
    bool Intersect(const Triangle& triangle, f32& t) const;
    bool Intersect(const Sphere& sphere, f32& t) const;
    bool Intersect(const Cylinder& capsule, float& t) const;
    bool Intersect(const Capsule& capsule, float& t) const;

    Vector3 ClosestPoint(const Vector3& point, float& t) const;
    float SqDistPoint(const Vector3& point) const;

    Vector3 o;
    Vector3 d;
};