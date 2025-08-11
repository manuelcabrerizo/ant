#pragma once

#include "collision_utils.h"

class Plane;
class Sphere;
class Capsule;
class OBB;
class Triangle;
class MeshCollider;

class AABB
{
private:
    Vector3 min;
    Vector3 max;
public:
    void Init(const Vector3& min, const Vector3& max);
    void Init(Array<Vector3>& points);

    void SetMin(const Vector3& min);
    void SetMax(const Vector3& max);
    Vector3 GetMin() const;
    Vector3 GetMax() const;

    bool Intersect(const AABB& aabb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const OBB& obb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Plane& plane, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Triangle& triangle, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const MeshCollider& meshCollider, Array<CollisionData>* collisionData = nullptr) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(const Vector3& color) const;
};