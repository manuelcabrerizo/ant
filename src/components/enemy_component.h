#pragma once

#include "component.h"
#include <artificial_intelligence/kinematic_behaviors.h>
#include <artificial_intelligence/steering_behaviors.h>

#include <notification_manager.h>

class TransformComponent;
class PhysicsComponent;
class AnimationComponent;
class ColliderComponent;

enum class EnemyAnimation
{
    Walk,
    Dead,

    Count
};

class EnemyComponent : public Component<EnemyComponent>, INotificable
{
private:
    TransformComponent* transform;
    PhysicsComponent* physics;
    AnimationComponent* animation;
    ColliderComponent* collider;

    Kinematic character;
    Kinematic target;
    SteeringWander wander;

    int maxLife = 3;
    int life = 3;

    void OnEnemyHit(EnemyHitNotification* enemyHit);
public:
    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification* notification) override;
};