#pragma once

#include "component.h"
#include <notification_manager.h>

class TransformComponent;

class WeaponComponent : public Component<WeaponComponent>, INotificable
{
private:
    TransformComponent *transform;
    SlotmapKey<INotificable *> shootNotification;
public:
    SlotmapKey<Actor> weapon;

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
};
