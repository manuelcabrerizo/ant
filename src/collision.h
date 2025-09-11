#pragma once

#include <collisions/collision_utils.h>
#include <collisions/collider.h>
#include <collisions/ray.h>
#include <collisions/segment.h>
#include <collisions/plane.h>

class CollisionWorld
{
private:
    CollisionWorld() {};
    static CollisionWorld instance;
    static bool isInitialized;

    Array<ColliderComponent *> colliders;
public:
    static void Init(int maxColllidersCount);
    static void Terminate();
    static CollisionWorld* Get();

    void AddCollider(ColliderComponent *collider);
    void RemoveCollider(ColliderComponent *collider);
    bool Intersect(const Ray& ray, Array<CollisionData>& collisionData, Actor* ignoreActor = nullptr) const;
    bool Intersect(const Segment& segment, Array<CollisionData>& collisionData, Actor* ignoreActor = nullptr) const;
    bool Intersect(const Collider& collider, Array<CollisionData>& collisionData) const;
    bool Intersect(ColliderComponent *collider, Array<CollisionData>& collisionData) const;
    void DebugDraw();
};