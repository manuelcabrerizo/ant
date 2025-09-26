#pragma once

#include "component.h"
#include <notification_manager.h>
#include <particle_systems/blood_ps.h>
#include <particle_systems/shoot_ps.h>

class TransformComponent;
class ColliderComponent;
class CameraComponent;

class WeaponComponent : public Component<WeaponComponent>, INotificable
{
private:
    TransformComponent* transform;
    ColliderComponent* collider;
    CameraComponent* camera;

    BloodParticleSystem bloodPs;
    ShootParticleSystem shootPs;

    float bloodDuration = 0.15f;
    float bloodTimer = 0.0f;

    float shootDuration = 0.05f;
    float shootTimer = 0.0f;

    float animationTime = 0.0f;
    float shootAnimationTime = 0.0f;

    float animationOffsetY = 0.0f;
    float animationOffsetX = 0.0f;
    float animationOffsetZ = 0.0f;

    void OnShoot(ShootNotification* notification);
public:
    Actor *weapon = nullptr;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification *notification) override;
    
    static void OnSetRenderState();
    void OnRender(ActorManager* actorManager);
};
