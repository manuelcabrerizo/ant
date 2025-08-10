#pragma once

#include "collision_utils.h"

class Sphere;
class Ray;
class Segment;
class Triangle;
class Plane;
class AABB;
class OBB;
class MeshCollider;

class Capsule
{
private:
    Vector3 a;
    Vector3 b;
    f32 r;
public:
    void Init(Vector3 p, Vector3 q, f32 r);

    void SetPosition(const Vector3& position);

    Vector3 GetA() const;
    Vector3 GetB() const;
    float GetRadio() const;

    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Triangle& triangle, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Plane& plane, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const AABB& aabb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const OBB& obb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const MeshCollider& meshCollider, Array<CollisionData>* collisionData = nullptr) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(int subdivision, const Vector3& color);
};