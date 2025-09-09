#include "effect_component.h"
#include "transform_component.h"
#include <asset_managers/texture_manager.h>

void EffectComponent::OnInit(ActorManager* actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::PlayerMove);


    transform = owner->GetComponent<TransformComponent>();

    TextureManager::Get()->Load("Flare", "data/textures/flare.png", FRAME_MEMORY);
    firePs.SetPosition(transform->position);
    firePs.SetTexture(TextureManager::Get()->Get("Flare"));
    firePs.SetTimeToSpawn(0.025f);
    firePs.Play();

}

void EffectComponent::OnTerminate(ActorManager* actorManager)
{
}

void EffectComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    if (playerTransform)
    {
        firePs.Update(playerTransform->position, dt);
    }
}

void EffectComponent::OnRender(ActorManager* actorManager)
{
    GraphicsManager::Get()->SetDepthStencilOnWriteMaskZero();
    GraphicsManager::Get()->SetAdditiveBlending();
    firePs.Render();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
}


void EffectComponent::OnPlayerMove(PlayerMoveNotification* playerMove)
{
    if (playerTransform == nullptr)
    {
        playerTransform = playerMove->transform;
    }
    else
    {
        NotificationManager::Get()->RemoveListener(this, NotificationType::PlayerMove);
    }
}

void EffectComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::PlayerMove: OnPlayerMove((PlayerMoveNotification*)notification); break;
    }
}

void EffectComponent::SetSpawnArea(const Vector3& min, const Vector3& max)
{
    firePs.SetSpawnArea(min, max);
}