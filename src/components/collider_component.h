#pragma once

#include "component.h"
#include <collisions/collider.h>

class TransformComponent;

class ColliderComponent : public Component<ColliderComponent>
{
private:
    Collider collider;
    Vector3 offset;
private:
    TransformComponent* transform = nullptr;

public:
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, f32 dt);

    Collider* GetCollider();
    Vector3 GetOffset();
    unsigned int GetId();

    void SetCollider(const Collider& collider);
    void SetOffset(const Vector3& offset);
};

