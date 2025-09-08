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
    static unsigned int gen;

    ColliderType type;
    unsigned int id;
    union
    {
        AABB aabb;
        OBB obb;
        Sphere sphere;
        Capsule capsule;
        MeshCollider meshCollider;
    };
    AABB volume;
public:

    Collider() {}
    Collider(const AABB& aabb);
    Collider(const OBB& obb);
    Collider(const Sphere& sphere);
    Collider(const Capsule& capsule);
    Collider(const MeshCollider& meshCollider);

    unsigned int GetId() const;
    void DebugDraw() const;

    bool Intersect(const Ray& ray, float& t) const;
    bool Intersect(const Segment& segment, float& t) const;
    bool Intersect(const Collider& other, Array<CollisionData>& collisionData) const;

    void UpdatePosition(const Vector3& position);
};