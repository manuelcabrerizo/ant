#include "portal_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include <collision.h>

void PortalComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();

    Vector3 size = Vector3::one;
    Vector3 offset = Vector3(0, 0.5f, 0);
    Vector3 position = transform->position + offset;
    AABB aabb;
    aabb.Init(size * -0.5f + position, size * 0.5f + position);
    collider = Collider(aabb);
}

void PortalComponent::OnTerminate(ActorManager* actorManager)
{
    
}

void PortalComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    isOnPortal = false;

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);
    CollisionWorld::Get()->Intersect(collider, collisionData);
    for (int i = 0; i < collisionData.size; i++)
    {
        if (collisionData[i].collider->owner->GetTag() == ActorTag::Player)
        {
            isOnPortal = true;
            break;
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);

    if (isOnPortal && !wasOnPortal)
    {
        OnPortalEnter();
    }
    else if (!isOnPortal && wasOnPortal)
    {
        OnPortalExit();
    }

    wasOnPortal = isOnPortal;
}

#include <windows.h>

void PortalComponent::OnPortalEnter()
{
    OutputDebugStringA("OnPortalEnter\n");
}

void PortalComponent::OnPortalExit()
{
    OutputDebugStringA("OnPortalExit\n");
}