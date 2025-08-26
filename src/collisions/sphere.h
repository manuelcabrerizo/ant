#pragma once

#include "collision_utils.h"

class Triangle;
class Ray;
class Segment;
class Plane;
class AABB;
class OBB;
class Capsule;
class MeshCollider;

class Sphere
{
private:
    Vector3 c;
    f32 r;
public:
    void Init(Vector3 c, f32 r);

    void SetCenter(const Vector3& center);

    Vector3 GetCenter() const;
    f32 GetRadio() const;
    
    bool Intersect(const Sphere& sphere, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Triangle& triangle, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Plane& plane, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const AABB& aabb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const OBB& obb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;

    bool DynamicIntersect(const Plane& plane, const Vector3& movement, f32& t) const;
    bool DynamicIntersect(const Triangle& triangle, const Vector3& movement, f32& t, Vector3& n) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(int subdivision, const Vector3& color) const;
};