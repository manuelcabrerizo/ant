#include "physics_component.h"
#include "transform_component.h"
#include "collider_component.h"
#include "notification_manager.h"

#include <collision.h>

void PhysicsComponent::OnInit(ActorManager *actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
}

void PhysicsComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessPhysics(dt);
    Array<Collider>& colliders = collider->GetColliders();
    for (int i = 0; i < colliders.size; i++)
    {
        colliders[i].UpdatePosition(transform->position + collider->GetOffset());
    }
    ProcessCollisionDetectionAndResolution();
}

void PhysicsComponent::ProcessPhysics(float dt)
{
    // Add the force of gravity
    forceAccumulator += Vector3(0.0f, -9.8f*3.0f, 0.0f);
    // Equations of motion
    Vector3 lastFrameAcceleration = acceleration;
    lastFrameAcceleration += forceAccumulator;
    velocity += lastFrameAcceleration * dt;
    velocity *= powf(0.01f, dt);
    transform->position += velocity * dt;
    forceAccumulator = Vector3(0.0f);
}

void PhysicsComponent::ProcessCollisionDetectionAndResolution()
{
    f32 colliderRadius = 0.25f;
    // Grounded test
    Segment groundSegment;
    Vector3 colliderPos = transform->position + collider->GetOffset();
    groundSegment.Init(colliderPos, colliderPos - Vector3(0.0f, colliderRadius + 0.80f, 0.0f));
    
    // Collision detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);

    grounded = CollisionWorld::Get()->Intersect(groundSegment, collisionData, owner);

    Array<CollisionData> contacts;
    contacts.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);

    collisionData.Clear();
    if(CollisionWorld::Get()->Intersect(collider, collisionData))
    {
        contacts.Clear();
        for (int i = 0; i < collisionData.size; i++)
        {
            if (collisionData[i].collider->IsTrigger() == false)
            {
                contacts.Push(collisionData[i]);
            }
        }

        int iterations = 0;
        while(contacts.size > 0 && iterations < 10)
        {
            if (owner->GetTag() == ActorTag::Player && contacts[0].collider->owner->GetTag() == ActorTag::Enemy)
            {
                EnemyHitPlayerNotification notification;
                notification.enemy = contacts[0].collider->owner;
                NotificationManager::Get()->SendNotification(NotificationType::EnemyHitPlayer, &notification);
            }

            Vector3 n = contacts[0].n;
            f32 penetration = contacts[0].penetration;
            transform->position += n * penetration;
            transform->position += n * 0.001f;
            acceleration = Vector3(0.0f);
            velocity -= n * velocity.Dot(n);
            Array<Collider>& colliders = collider->GetColliders();
            for (int i = 0; i < colliders.size; i++)
            {
                colliders[i].UpdatePosition(transform->position + collider->GetOffset());
            }

            collisionData.Clear();
            CollisionWorld::Get()->Intersect(collider, collisionData);
            contacts.Clear();
            for (int i = 0; i < collisionData.size; i++)
            {
                if (collisionData[i].collider->IsTrigger() == false)
                {
                    contacts.Push(collisionData[i]);
                }
            }

            iterations++;
        }    
    }

    MemoryManager::Get()->ReleaseFrame(frame);
}