#pragma once

#include <collisions/collision_utils.h>
#include <collisions/collider.h>
#include <collisions/ray.h>
#include <collisions/segment.h>
#include <collisions/plane.h>

class CollisionWorld
{
public:
    void Initialize(int maxColllidersCount);
    void DebugDraw();

    Collider *AddCollider(const Collider& collider);
    void RemoveCollider(const Collider& collider);

    bool Intersect(const Ray& ray, float& t, unsigned int ignoreId = 0) const;
    bool Intersect(const Segment& segment, float& t, unsigned int ignoreId = 0) const;
    bool Intersect(const Collider& collider, Array<CollisionData>& collisionData) const;
private:
    Array<Collider> colliders;
};