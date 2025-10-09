#include "enemy_dead_state.h"
#include <components/enemy_component.h>
#include <components/physics_component.h>
#include <components/collider_component.h>
#include <components/animation_component.h>

void EnemyDeadState::Init(EnemyComponent* owner)
{
    this->owner = owner;
}

void EnemyDeadState::OnEnter()
{
    owner->physics->enable = false;
    owner->collider->enable = false;
    owner->animation->Transition((int)EnemyAnimation::Dead, 0.25f);
    EnemyKillNotification notification;
    NotificationManager::Get()->SendNotification(NotificationType::EnemyKill, &notification);
}

void EnemyDeadState::OnExit()
{
}

void EnemyDeadState::OnUpdate(float deltaTime)
{
}