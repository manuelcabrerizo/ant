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
    for (int i = 0; i < colliders.size; i++)
    {
        colliders[i].UpdatePosition(transform->position + offset);
    }
}

void ColliderComponent::Init(int size, int memoryType)
{
    colliders.Init(size, memoryType);
    offset = Vector3::zero;
}

void ColliderComponent::AddSubCollider(const Collider& collider)
{
    colliders.Push(collider);
}

Array<Collider>& ColliderComponent::GetColliders()
{
    return colliders;
}

Vector3 ColliderComponent::GetOffset()
{
    return offset;
}

void ColliderComponent::SetOffset(const Vector3& offset)
{
    this->offset = offset;
}