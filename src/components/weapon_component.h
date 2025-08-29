#pragma once

#include "component.h"
#include <notification_manager.h>
#include <particle_system.h>

class TransformComponent;
class ColliderComponent;
class CameraComponent;

class WeaponComponent : public Component<WeaponComponent>, INotificable
{
private:
    TransformComponent* transform;
    ColliderComponent* collider;
    CameraComponent* camera;

    ActorManager* am = nullptr;

    Actor *bulletPrefab = nullptr;

    BloodParticleSystem particleSystem;
    float timeToSpawn = 0.05f;

    void OnShoot(ShootNotification* notification);
public:
    Actor *weapon = nullptr;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification *notification) override;
    void OnRender(ActorManager* actorManager);
};
