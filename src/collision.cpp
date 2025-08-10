#include <collision.h>
#include <math/algebra.h>
#include <float.h>
#include <collisions/collision_utils.h>

void CollisionWorld::Initialize(int maxColllidersCount)
{
    colliders.Init(maxColllidersCount, STATIC_MEMORY);
}

Collider *CollisionWorld::AddCollider(const Collider& collider)
{
    colliders.Push(collider);
    return &colliders[colliders.size - 1];
}

void CollisionWorld::RemoveCollider(const Collider& collider)
{
    // TODO: ...
}

bool CollisionWorld::Intersect(const Ray& ray, float& t, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (colliders[i].GetId() != ignoreId)
        {
            isIntersecting |= colliders[i].Intersect(ray, t);
        }
    }
    return isIntersecting;
}

bool CollisionWorld::Intersect(const Segment& segment, float& t, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (colliders[i].GetId() != ignoreId)
        {
            isIntersecting |= colliders[i].Intersect(segment, t);
        }
    }
    return isIntersecting;
}

bool CollisionWorld::Intersect(const Collider& collider, Array<CollisionData>& collisionData) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (collider.GetId() != colliders[i].GetId())
        {
            isIntersecting |= collider.Intersect(colliders[i], collisionData);
        }
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return isIntersecting;
}