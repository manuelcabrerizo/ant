#pragma once

#include <containers.h>

#include "aabb.h"
#include "obb.h"
#include "sphere.h"
#include "capsule.h"
#include "mesh_collider.h"

class Ray;
class Segment;
class MeshCollider;
class Actor;

enum class ColliderType
{
    AABB = 0,
    OBB,
    SPHERE,
    CAPSULE,
    MESH_COLLIDER
};

class Collider
{
    friend MeshCollider;
private:
    ColliderType type;
    union
    {
        AABB aabb;
        OBB obb;
        Sphere sphere;
        Capsule capsule;
        MeshCollider meshCollider;
    };
    AABB volume;
    Actor* owner = nullptr;
    Vector3 offset = Vector3::zero;
public:

    Collider() {}
    Collider(const AABB& aabb, Actor* actor);
    Collider(const OBB& obb, Actor* actor);
    Collider(const Sphere& sphere, Actor* actor);
    Collider(const Capsule& capsule, Actor* actor);
    Collider(const MeshCollider& meshCollider, Actor* actor);

    Actor* GetOwner() const;

    void DebugDraw() const;

    bool Intersect(const Ray& ray, float& t) const;
    bool Intersect(const Segment& segment, float& t) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>& collisionData) const;
    bool Intersect(const Collider& other, Array<CollisionData>& collisionData) const;

    void UpdatePosition(const Vector3& position);
    void SetOffset(const Vector3& offset);
};