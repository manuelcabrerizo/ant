#include "enemy_component.h"
#include "transform_component.h"
#include "physics_component.h"
#include "animation_component.h"
#include "collider_component.h"

#include <asset_managers/animation_manager.h>
#include <particle_systems/blood_ps.h>

#include <math.h>
#include <math/algebra.h>

void EnemyComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::EnemyHit);
    NotificationManager::Get()->AddListener(this, NotificationType::PlayerMove);

    transform = owner->GetComponent<TransformComponent>();
    physics = owner->GetComponent<PhysicsComponent>();
    animation = owner->GetComponent<AnimationComponent>();
    collider = owner->GetComponent<ColliderComponent>();

    // Initialize animation component
    animation->SetSkeleton(SkeletonManager::Get()->Get("Bloodwraith"));
    animation->AddAnimation((int)EnemyAnimation::Idle, AnimationManager::Get()->Get("Idle"), false);
    animation->AddAnimation((int)EnemyAnimation::Walk, AnimationManager::Get()->Get("Walking"), true);
    animation->AddAnimation((int)EnemyAnimation::Dead, AnimationManager::Get()->Get("Death"), false);
    animation->AddAnimation((int)EnemyAnimation::Attack, AnimationManager::Get()->Get("Attack"), true);
    animation->AddAnimation((int)EnemyAnimation::Hit, AnimationManager::Get()->Get("Hit"), false);
    animation->SetAnimation((int)EnemyAnimation::Idle);

    idleState.Init(this);
    walkState.Init(this);
    attackState.Init(this);
    hitState.Init(this);
    deadState.Init(this);
    states.Push(&idleState);

    life = maxLife;

    EnemySpawnNotification notification;
    NotificationManager::Get()->SendNotification(NotificationType::EnemySpawn, &notification);
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::PlayerMove);
    NotificationManager::Get()->RemoveListener(this, NotificationType::EnemyHit);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    states.Update(dt);
}

void EnemyComponent::OnEnemyHit(EnemyHitNotification* enemyHit)
{

    if (enemyHit->enemy == owner && states.Peek() != &hitState)
    {
        enemyHit->ps->Play();
        *enemyHit->bloodTimer = enemyHit->bloodDuration;

        if ((life - 1) == 0)
        {
            states.ChangeState(&deadState);
        }
        else
        {
            states.ChangeState(&hitState);
        }
        life--;
    }
}

void EnemyComponent::OnPlayerMove(PlayerMoveNotification* playerMove)
{
    target.position = playerMove->position;
    target.velocity = playerMove->velocity;
}

void EnemyComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::EnemyHit: OnEnemyHit((EnemyHitNotification*)notification); break;
    case NotificationType::PlayerMove: OnPlayerMove((PlayerMoveNotification*)notification); break;
    }
}