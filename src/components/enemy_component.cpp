#include "enemy_component.h"
#include "transform_component.h"
#include "physics_component.h"
#include "animation_component.h"
#include "collider_component.h"

#include <asset_managers/animation_manager.h>

#include <math.h>
#include <math/algebra.h>

void EnemyComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::EnemyHit);

    transform = owner->GetComponent<TransformComponent>();
    physics = owner->GetComponent<PhysicsComponent>();
    animation = owner->GetComponent<AnimationComponent>();
    collider = owner->GetComponent<ColliderComponent>();

    // Initialize animation component
    animation->SetSkeleton(SkeletonManager::Get()->Get("Bloodwraith"));
    animation->AddAnimation((int)EnemyAnimation::Walk, AnimationManager::Get()->Get("Walking"), true, 0.5f);
    animation->AddAnimation((int)EnemyAnimation::Dead, AnimationManager::Get()->Get("Death"), false);
    animation->SetAnimation((int)EnemyAnimation::Walk);

    wander.SetCharacter(&character);
    wander.SetTarget(&target);
    wander.SetWanderOffset(1.0f);
    wander.SetWanderRadius(5.0f);
    wander.SetWanderRate(0.5f);
    wander.SetWanderOrientation(0.0f);
    wander.SetMaxAcceleration(2.5f);
    wander.SetMaxAngularAcceleration(5.0f);
    wander.SetMaxRotation(10.0f);
    wander.SetTargetRadius(ANT_PI * 0.1f);
    wander.SetSlowRadius(ANT_PI * 0.5f);
    wander.SetTimeToTarget(0.001f);

    life = maxLife;
}

void EnemyComponent::OnTerminate(ActorManager *actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::EnemyHit);
}

void EnemyComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    if(physics->grounded && life > 0)
    {
        character.position = transform->position;
        character.velocity = physics->velocity;
        character.velocity.y = 0.0f;
        SteeringOutput steering = wander.GetSteering();
        physics->acceleration = steering.linear;
        character.rotation += steering.angular * dt;
        character.rotation *= powf(0.001f, dt); // angular drag
        character.orientation += character.rotation * dt;
        transform->direction = Vector3(-sinf(character.orientation), 0.0f, cosf(character.orientation)).Normalized();
    }
}

void EnemyComponent::OnEnemyHit(EnemyHitNotification* enemyHit)
{
    if (enemyHit->enemy == owner)
    {
        if ((life - 1) == 0)
        {
            physics->enable = false;
            collider->enable = false;
            animation->Transition((int)EnemyAnimation::Dead, 0.25f);
        }
        life--;
    }
}

void EnemyComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::EnemyHit: OnEnemyHit((EnemyHitNotification*)notification); break;
    }
}