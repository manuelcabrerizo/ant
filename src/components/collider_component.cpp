#include "collider_component.h"
#include <notification_manager.h>
#include "transform_component.h"
#include <collision.h>

void ColliderComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    CollisionWorld::Get()->AddCollider(this);
}

void ColliderComponent::OnTerminate(ActorManager* actorManager)
{
    CollisionWorld::Get()->RemoveCollider(this);
}

void ColliderComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    //collider.UpdatePosition(transform->position + offset);
}

Collider* ColliderComponent::GetCollider()
{
    return &collider;
}

Vector3 ColliderComponent::GetOffset()
{
    return offset;
}

unsigned int ColliderComponent::GetId()
{
    return collider.GetId();
}

void ColliderComponent::SetCollider(const Collider& collider)
{
    this->collider = collider;
}
void ColliderComponent::SetOffset(const Vector3& offset)
{
    this->offset = offset;
}