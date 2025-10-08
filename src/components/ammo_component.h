#pragma once

#include "component.h"
class TransformComponent;
class ColliderComponent;
class RenderComponent;

class AmmoComponent : public Component<AmmoComponent>
{
private:
    TransformComponent* transform = nullptr;
    ColliderComponent* collider = nullptr;
    RenderComponent* render = nullptr;
    float timer = 0.0f;
    float timeToReset = 15.0f;
    float resetTimer = 0;
    bool isGrabbed = false;
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);
    void Grab();
};