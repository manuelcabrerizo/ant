#pragma once

#include "component.h"
#include <particle_systems/fire_ps.h>

#include <notification_manager.h>


class TransformComponent;


class EffectComponent : public Component<EffectComponent>, INotificable
{
private:
    TransformComponent* transform = nullptr;
    FireParticleSystem firePs;

    TransformComponent* playerTransform = nullptr;

    void OnPlayerMove(PlayerMoveNotification* playerMove);
public:
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, f32 dt);
    void OnRender(ActorManager* actorManager);
    void OnNotify(NotificationType type, Notification* notification) override;

    void SetSpawnArea(const Vector3& min, const Vector3& max);
};