#pragma once

#include "collision_utils.h"

class Triangle;
class Sphere;
class Cylinder;
class Capsule;
class Plane;
class AABB;
class OBB;
class MeshCollider;

class Ray
{
private:
    Vector3 o;
    Vector3 d;

public:
    void Init(Vector3 o, Vector3 d);
    bool Intersect(const Triangle& triangle, f32& t) const;
    bool Intersect(const Sphere& sphere, f32& t) const;
    bool Intersect(const Cylinder& capsule, float& t) const;
    bool Intersect(const Capsule& capsule, float& t) const;
    bool Intersect(const Plane& plane, float& t) const;
    bool Intersect(const AABB& aabb, float& t) const;
    bool Intersect(const OBB& obb, float& t) const;

    Vector3 ClosestPoint(const Vector3& point, float& t) const;
    float SqDistPoint(const Vector3& point) const;

    Vector3 GetOrigin() const;
    Vector3 GetDirection() const;
};