#pragma once

#include "component.h"
#include <notification_manager.h>

class TransformComponent;

class WeaponComponent : public Component<WeaponComponent>, INotificable
{
private:
    TransformComponent *transform;
    ActorManager* am = nullptr;

    SlotmapKey<Actor> bulletPrefab;

    void OnShoot(ShootNotification* notification);
public:
    SlotmapKey<Actor> weapon;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification *notification) override;
};
