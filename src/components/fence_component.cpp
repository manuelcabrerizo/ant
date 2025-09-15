#include "fence_component.h"
#include "collider_component.h"
#include "transform_component.h"
#include <collision.h>

void FenceComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    triggerCount = 0;

    isOpen = true;

    if (IsOpen())
    {
        transform->position = openPosition;
    }
    else
    {
        transform->position = closePosition;
    }
}

void FenceComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    if (isOneTimeTrigger && triggerCount > 0)
    {
        return;
    }

    isOnTrigger = false;
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);
    CollisionWorld::Get()->Intersect(trigger, collisionData);
    for (int i = 0; i < collisionData.size; i++)
    {
        if (collisionData[i].collider->owner->GetTag() == ActorTag::Player)
        {
            isOnTrigger = true;
            if (isOnTrigger && !wasOnTrigger)
            {
                OnTriggerEnter(collisionData[i].collider->owner);
                triggerCount++;
            }
            break;
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
    wasOnTrigger = isOnTrigger;

    trigger.DebugDraw();
}

void FenceComponent::OnTriggerEnter(Actor* actor)
{
    isOpen = !isOpen;
    if (IsOpen())
    {
        transform->position = openPosition;
    }
    else
    {
        transform->position = closePosition;
    }
}

bool FenceComponent::IsOpen()
{
    return isOpen;
}

void FenceComponent::SetOpenPosition(const Vector3& position)
{
    this->openPosition = position;
}

void FenceComponent::SetClosePosition(const Vector3& position)
{
    this->closePosition = position;
}

void FenceComponent::SetIsOpen(bool isOpen)
{
    this->isOpen = isOpen;
}

void FenceComponent::SetTrigger(const Collider& trigger)
{
    this->trigger = trigger;
}

void FenceComponent::SetIsOneTimeTrigger(bool value)
{
    this->isOneTimeTrigger = value;
}