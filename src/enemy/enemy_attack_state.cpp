#include "enemy_attack_state.h"
#include <components/enemy_component.h>
#include <components/transform_component.h>
#include <components/animation_component.h>
#include <components/collider_component.h>
#include <collision.h>

void EnemyAttackState::Init(EnemyComponent* owner)
{
    this->owner = owner;
}

void EnemyAttackState::OnEnter()
{
    owner->animation->Transition((int)EnemyAnimation::Attack, 0.25f);
    animationNode = owner->animation->GetNextAnimation();
    animationNode->time = 0.0f;
    isPlayerHit = false;
}

void EnemyAttackState::OnExit()
{
}

void EnemyAttackState::OnUpdate(float deltaTime)
{
    float distToTargetSq = (owner->transform->position - owner->target.position).MagnitudeSq();
    float stopAttackRadio = owner->attackRadio * 1.5f;
    if (distToTargetSq > stopAttackRadio * stopAttackRadio)
    {
        owner->states.ChangeState(&owner->walkState);
    }

    float time = animationNode->time;
    float duration = animationNode->animation->GetDuration();
    if (!isPlayerHit)
    {
        if (time > duration * 0.3f)
        {

            TryAttack();
            isPlayerHit = true;
        }
    }

    if (time >= duration - 0.1f)
    {
        animationNode->time = 0.0f;
        isPlayerHit = false;
    }
}

void EnemyAttackState::TryAttack()
{
    Matrix4 rotMat = Matrix4::TransformFromEuler(owner->transform->rotation);
    Vector3 front = Matrix4::TransformVector(rotMat, Vector3::forward);

    Vector3 pos = owner->transform->position + front * 1.5f;
    float radio = 2;

    Sphere attackArea;
    attackArea.Init(pos, radio);

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);
    collisionData.Clear();
    if (CollisionWorld::Get()->Intersect(attackArea, collisionData))
    {
        for (int i = 0; i < collisionData.size; i++)
        {
            Actor* other = collisionData[i].collider->owner;
            if (other->GetTag() == ActorTag::Player)
            {
                PlayerHitNotification notification;
                notification.actor = owner->owner;
                notification.hitAmount = 40;
                NotificationManager::Get()->SendNotification(NotificationType::PlayerHit, &notification);
            }
        }
    }

    MemoryManager::Get()->ReleaseFrame(frame);
}