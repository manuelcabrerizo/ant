#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>

class TransformComponent;
class ColliderComponent;

class PortalComponent : public Component<PortalComponent>
{
private:
    Collider trigger;
    TransformComponent* transform = nullptr;

    bool isOnPortal = false;
    bool wasOnPortal = false;
    Vector3 dstPos;
    float dstRot;

    void OnPortalEnter(Actor* actor);
    void OnPortalExit();
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);

    void SetDestination(const Vector3& dstPos, float dstRot);
};