#include "enemy_idle_state.h"
#include <components/enemy_component.h>
#include <components/animation_component.h>
#include <components/transform_component.h>

void EnemyIdleState::Init(EnemyComponent* owner)
{
    this->owner = owner;
}

void EnemyIdleState::OnEnter()
{
    owner->animation->Transition((int)EnemyAnimation::Idle, 0.5f);
}

void EnemyIdleState::OnExit()
{
}

void EnemyIdleState::OnUpdate(float deltaTime)
{
    float distToTargetSq = (owner->transform->position - owner->target.position).MagnitudeSq();
    if (distToTargetSq <= owner->detectionRadio * owner->detectionRadio)
    {
        owner->states.ChangeState(&owner->walkState);
    }
}

