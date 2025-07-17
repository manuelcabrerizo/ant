#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;
class Plane;
class AABB;
class OBB;
class Capsule;

class Sphere
{
private:
    Vector3 c;
    f32 r;
public:
    void Init(Vector3 c, f32 r);

    Vector3 GetCenter() const;
    f32 GetRadio() const;

    bool Intersect(const Sphere& sphere) const;
    bool Intersect(const Triangle& triangle, Vector3& n, f32& penetration) const;
    bool Intersect(const Plane& plane) const;
    bool Intersect(const AABB& aabb) const;
    bool Intersect(const OBB& obb) const;
    bool Intersect(const Capsule& capsule) const;


    bool DynamicIntersect(const Plane& plane, const Vector3& movement, f32& t) const;
    bool DynamicIntersect(const Triangle& triangle, const Vector3& movement, f32& t, Vector3& n) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;
};