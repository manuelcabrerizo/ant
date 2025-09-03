#include <collision.h>
#include <math/algebra.h>
#include <float.h>
#include <collisions/collision_utils.h>
#include <components/collider_component.h>

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

void CollisionWorld::AddCollider(ColliderComponent*collider)
{
    colliders.Push(collider);
}

void CollisionWorld::RemoveCollider(ColliderComponent *collider)
{
    for (int i = colliders.size - 1; i >= 0; --i)
    {
        if (colliders[i] == collider)
        {
            colliders[i] = colliders[colliders.size - 1];
            colliders[colliders.size - 1] = nullptr;
            colliders.size--;

            break;
        }
    }
}

bool CollisionWorld::Intersect(const Ray& ray, Array<CollisionData>& collisionData, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (!colliders[i]->enable)
        {
            continue;
        }

        Collider* collider = colliders[i]->GetCollider();
        if (collider->GetId() != ignoreId)
        {
            float t;
            bool intersect = collider->Intersect(ray, t);
            if (intersect && collisionData.size < MAX_COLLISION_COUNT)
            {
                if (t < 0.0f)
                {
                    int StopHere = 0;
                }
                CollisionData data;
                data.collider = colliders[i];
                data.t = t;
                collisionData.Push(data);
            }
            isIntersecting |= intersect;
        }
    }

    if (collisionData.size > 0)
    {
        CollisionUtils::SortCollisionByTime(collisionData);
    }

    return isIntersecting;
}

bool CollisionWorld::Intersect(const Segment& segment, Array<CollisionData>& collisionData, unsigned int ignoreId) const
{
    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (!colliders[i]->enable)
        {
            continue;
        }

        Collider* collider = colliders[i]->GetCollider();
        if (collider->GetId() != ignoreId)
        {
            float t;
            bool intersect = collider->Intersect(segment, t);
            if (intersect && collisionData.size < MAX_COLLISION_COUNT)
            {
                CollisionData data;
                data.collider = colliders[i];
                data.t = t;
                collisionData.Push(data);
            }
            isIntersecting |= intersect;
        }
    }

    if (collisionData.size > 0)
    {
        CollisionUtils::SortCollisionByTime(collisionData);
    }

    return isIntersecting;
}

bool CollisionWorld::Intersect(ColliderComponent *collider, Array<CollisionData>& collisionData) const
{
    if (!collider->enable)
    {
        return false;
    }

    bool isIntersecting = false;
    for (int i = 0; i < colliders.size; ++i)
    {
        if (!colliders[i]->enable)
        {
            continue;
        }

        if (collider->GetId() != colliders[i]->GetId())
        {
            bool intersect = collider->GetCollider()->Intersect(*colliders[i]->GetCollider(), collisionData);
            if (intersect)
            {
                CollisionData& data = collisionData[collisionData.size - 1];
                data.collider = colliders[i];
            }
            isIntersecting |= intersect;
        }
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return isIntersecting;
}

void CollisionWorld::DebugDraw()
{
    for (int i = 0; i < colliders.size; ++i)
    {
        colliders[i]->GetCollider()->DebugDraw();
    }
}