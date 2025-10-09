#include "heal_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "render_component.h"
#include <math/algebra.h>

void HealComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
    render = owner->GetComponent<RenderComponent>();

    collider->enable = true;
    collider->SetIsTrigger(true);

    timer = 0.0f;
    resetTimer = 0.0f;
    isGrabbed = false;
}

void HealComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    if (isGrabbed)
    {
        if (resetTimer <= 0.0f)
        {
            isGrabbed = false;
            collider->enable = true;
            render->enable = true;
            resetTimer = 0.0f;
        }
        else
        {
            resetTimer -= dt;
        }
    }

    transform->rotation.y = timer;
    if (timer > (ANT_PI * 2.0f))
    {
        timer = 0;
    }
    timer += dt;
}

void HealComponent::Grab()
{
    collider->enable = false;
    render->enable = false;
    isGrabbed = true;
    resetTimer = timeToReset;
}