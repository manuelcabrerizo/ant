#pragma once
#include "component.h"
#include "transform_component.h"

class BulletComponent : public Component<BulletComponent>
{
private:
    float lifeTime = 5.0f;
    float currentLifeTime = 0.0f;
    float speed = 20.0f;
public:
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, float dt);
};

