#pragma once

#include "component.h"
#include <collisions/collider.h>

class TransformComponent;

class KeyComponent : public Component<KeyComponent>
{
private:
    Collider trigger;
    TransformComponent* transform = nullptr;

    bool isGrabbed = false;
    bool wasGrabbed = false;

    void OnGrabbed();
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);
};