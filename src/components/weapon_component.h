#pragma once

#include "component.h"
#include <notification_manager.h>

class TransformComponent;
class ColliderComponent;

class WeaponComponent : public Component<WeaponComponent>, INotificable
{
private:
    TransformComponent* transform;
    ColliderComponent* collider;

    ActorManager* am = nullptr;

    Actor *bulletPrefab = nullptr;

    void OnShoot(ShootNotification* notification);
public:
    Actor *weapon = nullptr;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification *notification) override;
};
