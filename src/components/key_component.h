#pragma once

#include "component.h"
class TransformComponent;
class ColliderComponent;

class KeyComponent : public Component<KeyComponent>
{
private:
    TransformComponent* transform = nullptr;
    ColliderComponent* collider = nullptr;

    float timer = 0.0f;

public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);
};