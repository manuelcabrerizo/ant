#include "key_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include <math/algebra.h>

void KeyComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
    collider->SetIsTrigger(true);
}

void KeyComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    transform->rotation.y = timer;
    if (timer > (ANT_PI * 2.0f))
    {
        timer = 0;
    }
    timer += dt;
}