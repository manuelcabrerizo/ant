#pragma once

#include "component.h"
#include <artificial_intelligence/kinematic_behaviors.h>
#include <artificial_intelligence/steering_behaviors.h>

#include <notification_manager.h>

class TransformComponent;
class PhysicsComponent;
class RenderComponent;

class EnemyComponent : public Component<EnemyComponent>, INotificable
{
private:
    TransformComponent* transform;
    PhysicsComponent* physics;
    RenderComponent* render;

    Kinematic character;
    Kinematic target;
    //SteeringPursue movement;
    //SteeringLookWhereYoureGoing turn;
    SteeringWander wander;

    void OnEnemyHit(EnemyHitNotification* enemyHit);
public:
    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification* notification) override;
};