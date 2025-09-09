#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>

class TransformComponent;

class PortalComponent : public Component<PortalComponent>
{
private:
    Collider collider;
    TransformComponent* transform = nullptr;

    bool isOnPortal = false;
    bool wasOnPortal = false;
    Vector3 destination;

    void OnPortalEnter(Actor* actor);
    void OnPortalExit();
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);

    void SetDestination(const Vector3& destination);
};