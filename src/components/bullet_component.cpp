#include "bullet_component.h"

void BulletComponent::OnInit(ActorManager* actorManager)
{
    currentLifeTime = lifeTime;
}

void BulletComponent::OnTerminate(ActorManager* actorManager)
{
}

void BulletComponent::OnUpdate(ActorManager* actorManager, float dt)
{
    TransformComponent *transform = actorManager->GetComponent<TransformComponent>(owner);
    transform->position += transform->direction * (speed * dt);

    currentLifeTime -= dt;
    if (currentLifeTime <= 0.0f)
    {
        actorManager->DestroyActor(owner);
    }
}