#include "portal_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "player_controller_component.h"
#include "physics_component.h"
#include <collision.h>

void PortalComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();

    // TODO: data driven this Collider Data
    ColliderComponent colliderComponent;
    colliderComponent.Init(3, FRAME_MEMORY);
    colliderComponent.SetOffset(Vector3(0, 1, 0));

    Vector3 front = transform->direction;
    Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
    Vector3 up = Vector3::Cross(front, right);

    Vector3 rotation[] = { right, up, front };
    
    // Left Collider
    {
        OBB obb;
        obb.Init(Vector3::zero, rotation, Vector3(0.5, 2, 0.5));
        Collider collider_ = Collider(obb, owner);
        collider_.SetOffset(up * 1.f + right * -1.5);
        colliderComponent.AddSubCollider(collider_);
    }
    
    // Right Collider
    {
        OBB obb;
        obb.Init(Vector3::zero, rotation, Vector3(0.5, 2, 0.5));
        Collider collider_ = Collider(obb, owner);
        collider_.SetOffset(up * 1.f + right * 1.5);
        colliderComponent.AddSubCollider(collider_);
    }

    // Up Collider
    {
        OBB obb;
        obb.Init(Vector3::zero, rotation, Vector3(2, 0.5, 0.5));
        Collider collider_ = Collider(obb, owner);
        collider_.SetOffset(up * 3.f);
        colliderComponent.AddSubCollider(collider_);
    }

    actorManager->AddComponent(owner, colliderComponent);

    collider = owner->GetComponent<ColliderComponent>();

    Vector3 size = Vector3::one;
    Vector3 offset = Vector3(0, 0.5f, 0);
    Vector3 position = transform->position + offset;
    AABB aabb;
    aabb.Init(size * -0.5f + position, size * 0.5f + position);
    trigger = Collider(aabb, owner);
}

void PortalComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    isOnPortal = false;

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);
    CollisionWorld::Get()->Intersect(trigger, collisionData);
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

    trigger.DebugDraw();
}

void PortalComponent::OnPortalEnter(Actor* actor)
{
    // TODO: mabye cache the player components the first time its enter the portal
    // or set all the values from a setter of the player controller component
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