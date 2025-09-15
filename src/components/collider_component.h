#pragma once

#include "component.h"
#include <collisions/collider.h>

class TransformComponent;

class ColliderComponent : public Component<ColliderComponent>
{
private:
    Array<Collider> colliders;
    Vector3 offset;
    bool isTrigger = false;
private:
    TransformComponent* transform = nullptr;

public:
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, f32 dt);

    void Init(int size, int memoryType);
    void AddSubCollider(const Collider& collider);
    Array<Collider>& GetColliders();
    Vector3 GetOffset();
    
    void SetOffset(const Vector3& offset);
    void SetIsTrigger(bool value);
    bool IsTrigger() const;
};

