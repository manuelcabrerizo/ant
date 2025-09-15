#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>

class TransformComponent;

class FenceComponent : public Component<FenceComponent>
{
private:
    TransformComponent* transform;

    Collider trigger;

    Vector3 openPosition;
    Vector3 closePosition;
    
    bool isOpen = false;

    bool isOneTimeTrigger = false;
    bool isOnTrigger = false;
    bool wasOnTrigger = false;
    int triggerCount = 0;

    void OnTriggerEnter(Actor* actor);
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);

    bool IsOpen();

    void SetOpenPosition(const Vector3& position);
    void SetClosePosition(const Vector3& position);
    void SetIsOpen(bool isOpen);
    void SetTrigger(const Collider& trigger);
    void SetIsOneTimeTrigger(bool value);
};