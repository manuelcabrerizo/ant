#include "portal_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "player_controller_component.h"
#include "physics_component.h"
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
            if (isOnPortal && !wasOnPortal)
            {
                OnPortalEnter(collisionData[i].collider->owner);
            }
            break;
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);

    
    if (!isOnPortal && wasOnPortal)
    {
        OnPortalExit();
    }

    wasOnPortal = isOnPortal;
}

void PortalComponent::OnPortalEnter(Actor* actor)
{
    TransformComponent* actorTransform = actor->GetComponent<TransformComponent>();
    PlayerControllerComponent* playerController = actor->GetComponent<PlayerControllerComponent>();
    PhysicsComponent* physics = actor->GetComponent<PhysicsComponent>();
    actorTransform->position = dstPos;
    playerController->SetRotation(dstRot);
    physics->velocity = Vector3::zero;
    physics->acceleration = Vector3::zero;
}

void PortalComponent::OnPortalExit()
{
}

void PortalComponent::SetDestination(const Vector3& dstPos, float dstRot)
{
    this->dstPos = dstPos;
    this->dstRot = dstRot;
}