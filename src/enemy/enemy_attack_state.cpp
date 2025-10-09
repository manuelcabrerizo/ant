#include "enemy_attack_state.h"
#include <components/enemy_component.h>
#include <components/transform_component.h>
#include <components/animation_component.h>

void EnemyAttackState::Init(EnemyComponent* owner)
{
    this->owner = owner;
}

void EnemyAttackState::OnEnter()
{
    owner->animation->Transition((int)EnemyAnimation::Attack, 0.25f);
}

void EnemyAttackState::OnExit()
{
}

void EnemyAttackState::OnUpdate(float deltaTime)
{
    float distToTargetSq = (owner->transform->position - owner->target.position).MagnitudeSq();
    if (distToTargetSq > owner->attackRadio * owner->attackRadio)
    {
        owner->states.ChangeState(&owner->walkState);
    }
}