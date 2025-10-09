#pragma once

#include "component.h"
#include <artificial_intelligence/kinematic_behaviors.h>
#include <artificial_intelligence/steering_behaviors.h>

#include <notification_manager.h>

#include <states/state_machine.h>
#include <enemy/enemy_attack_state.h>
#include <enemy/enemy_dead_state.h>
#include <enemy/enemy_hit_state.h>
#include <enemy/enemy_idle_state.h>
#include <enemy/enemy_walk_state.h>

class TransformComponent;
class PhysicsComponent;
class AnimationComponent;
class ColliderComponent;

enum class EnemyAnimation
{
    Idle,
    Walk,
    Dead,
    Attack,
    Hit,

    Count,
};

class EnemyComponent : public Component<EnemyComponent>, INotificable
{
    friend EnemyIdleState;
    friend EnemyWalkState;
    friend EnemyAttackState;
    friend EnemyHitState;
    friend EnemyDeadState;

private:

    StateMachine states;
    EnemyIdleState idleState;
    EnemyWalkState walkState;
    EnemyAttackState attackState;
    EnemyHitState hitState;
    EnemyDeadState deadState;

    TransformComponent* transform;
    PhysicsComponent* physics;
    AnimationComponent* animation;
    ColliderComponent* collider;

    Kinematic character;
    Kinematic target;
    SteeringPursue pursue;
    SteeringFace face;

    int maxLife = 3;
    int life = 3;
    float attackRadio = 2.5f;
    float detectionRadio = 30.0f;

    void OnEnemyHit(EnemyHitNotification* enemyHit);
    void OnPlayerMove(PlayerMoveNotification* playerMove);
public:
    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, Notification* notification) override;
};