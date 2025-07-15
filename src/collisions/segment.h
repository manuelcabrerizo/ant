#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Sphere;
class Cylinder;
class Capsule;

class Segment
{
public:
    void Init(Vector3 a, Vector3 b);
    bool Intersect(const Triangle& triangle, f32& t) const;
    bool Intersect(const Sphere& sphere, f32& t) const;
    bool Intersect(const Cylinder& cylinder, float& t) const;
    bool Intersect(const Capsule& capsule, float& t) const;

    Vector3 ClosestPoint(const Vector3& point, f32& t) const;
    float SqDistPoint(const Vector3& point) const;

    Vector3 a;
    Vector3 b;
};
