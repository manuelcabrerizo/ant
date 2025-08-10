#pragma once

#include "collision_utils.h"
#include <math/matrix4.h>

class AABB;
class Plane;
class Sphere;
class Capsule;
class Triangle;
class MeshCollider;

class OBB
{
private:
    Vector3 c;
    Vector3 u[3];
    Vector3 e;
public:
    void Init(const Vector3& center, const Vector3 orientation[], const Vector3& extent);

    void SetCenter(const Vector3& center);

    Vector3 GetCenter() const;
    Vector3 GetOrientation(int axis) const;
    Vector3 GetExtent() const;

    bool Intersect(const OBB& two, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const AABB& aabb, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const Plane& plane, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const Triangle& triangle, Array<CollisionData> *collisionData = nullptr) const;
    bool Intersect(const  MeshCollider& meshCollider, Array<CollisionData>* collisionData = nullptr) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(const Vector3& color);

private:

    Matrix4 GetTransform() const;
    bool Intersect(const Plane& plane, CollisionData* collisionData, float& smallestPenetration) const;

    static float ProjectToAxis(const OBB& obb, const Vector3& axis);

    static float PenetraionOnAxis(const OBB& one, const OBB& two,
        const Vector3& axis, const Vector3& toCenter);

    static bool TryAxis(const OBB& one, const OBB& two,
        Vector3 axis, const Vector3& toCenter, int index,
        float& smallestPrenetration, int& smallestCase);

    static Vector3 ContactPoint(
        const Vector3& pOne, const Vector3& dOne, float oneSize,
        const Vector3& pTwo, const Vector3& dTwo, float twoSize,
        bool useOne);
};