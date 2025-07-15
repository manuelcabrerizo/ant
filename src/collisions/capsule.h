#pragma once

#include <common.h>
#include <math/vector3.h>

class Sphere;
class Ray;
class Segment;
class Triangle;
class Plane;
class AABB;
class OBB;

class Capsule
{
private:
    Vector3 a;
    Vector3 b;
    f32 r;
public:
    void Init(Vector3 p, Vector3 q, f32 r);

    Vector3 GetA() const;
    Vector3 GetB() const;
    float GetRadio() const;

    bool Intersect(const Capsule& capsule) const;
    bool Intersect(const Sphere& sphere) const;
    bool Intersect(const Triangle& triangle, Vector3& n, float& penetration) const;
    bool Intersect(const Plane& plane) const;
    bool Intersect(const AABB& aabb);
    bool Intersect(const OBB& obb);

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(int subdivision, const Vector3& color);
};