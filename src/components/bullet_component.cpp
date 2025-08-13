#include "bullet_component.h"

void BulletComponent::OnInit(ActorManager* actorManager)
{
    currentLifeTime = lifeTime;
    Actor* actor = actorManager->GetActor(owner);
    transform = actor->GetComponent<TransformComponent>();
}

void BulletComponent::OnTerminate(ActorManager* actorManager)
{
}

void BulletComponent::OnUpdate(ActorManager* actorManager, float dt)
{
    transform->position += transform->direction * (speed * dt);

    currentLifeTime -= dt;
    if (currentLifeTime <= 0.0f)
    {
        actorManager->DestroyActor(owner);
    }
}