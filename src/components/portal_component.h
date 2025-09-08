#pragma once

#include "component.h"
#include <collisions/collider.h>

class TransformComponent;

class PortalComponent : public Component<PortalComponent>
{
private:
    Collider collider;
    TransformComponent* transform = nullptr;
    bool isOnPortal = false;
    bool wasOnPortal = false;


    void OnPortalEnter();
    void OnPortalExit();
public:
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, f32 dt);
};

