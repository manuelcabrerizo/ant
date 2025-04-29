#pragma once

#include "artificial_intelligence/kinematic_behaviors.h"

class EnemyComponent : public Component<EnemyComponent>, INotificable
{
private:
    TransformComponent *transform;
    PhysicsComponent *physics;

    Kinematic character;
    Kinematic target;
    KinematicWander movement;

    SlotmapKey<INotificable *> playerMoveNotification;
public:
    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
};