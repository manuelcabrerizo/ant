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

    Array<Collider *> colliders;
public:
    static void Init(int maxColllidersCount);
    static void Terminate();
    static CollisionWorld* Get();

    void AddCollider(Collider *collider);
    void RemoveCollider(Collider *collider);
    bool Intersect(const Ray& ray, float& t, unsigned int ignoreId = 0) const;
    bool Intersect(const Segment& segment, float& t, unsigned int ignoreId = 0) const;
    bool Intersect(const Collider& collider, Array<CollisionData>& collisionData) const;
    void DebugDraw();
};