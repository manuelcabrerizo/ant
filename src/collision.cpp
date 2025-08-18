#include <collision.h>
#include <math/algebra.h>
#include <float.h>
#include <collisions/collision_utils.h>

CollisionWorld CollisionWorld::instance;
bool CollisionWorld::isInitialized = false;

void CollisionWorld::Init(int maxColllidersCount)
{
    if (isInitialized)
    {
        ASSERT(!"Error: collision world already initialize");
    }
    instance.colliders.Init(maxColllidersCount, STATIC_MEMORY);
    isInitialized = true;
}

void CollisionWorld::Terminate()
{
    if (!isInitialized)
    {
        ASSERT(!"Error: collision world has not been initialize");
    }
    isInitialized = false;
}

CollisionWorld* CollisionWorld::Get()
{
    if (!isInitialized)
    {
        ASSERT(!"Error: collision world has not been initialize");
    }
    return &instance;
}

void CollisionWorld::AddCollider(Collider *collider)
{
    colliders.Push(collider);
}

void CollisionWorld::RemoveCollider(Collider *collider)
{
    for (int i = colliders.size - 1; i >= 0; --i)
    {
        if (colliders[i] == collider)
        {
            colliders[i] = colliders[colliders.size - 1];
            colliders[colliders.size - 1] = nullptr;
            colliders.size--;
        }
    }
}

bool CollisionWorld::Intersect(const Ray& ray, float& t, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (colliders[i]->GetId() != ignoreId)
        {
            isIntersecting |= colliders[i]->Intersect(ray, t);
        }
    }
    return isIntersecting;
}

bool CollisionWorld::Intersect(const Segment& segment, float& t, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (colliders[i]->GetId() != ignoreId)
        {
            isIntersecting |= colliders[i]->Intersect(segment, t);
        }
    }
    return isIntersecting;
}

bool CollisionWorld::Intersect(const Collider& collider, Array<CollisionData>& collisionData) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (collider.GetId() != colliders[i]->GetId())
        {
            isIntersecting |= collider.Intersect(*colliders[i], collisionData);
        }
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return isIntersecting;
}

void CollisionWorld::DebugDraw()
{
    for (int i = 0; i < colliders.size; ++i)
    {
        colliders[i]->DebugDraw();
    }
}