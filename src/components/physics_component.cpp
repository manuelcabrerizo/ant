#include "physics_component.h"
#include "transform_component.h"
#include "collider_component.h"

#include <collision.h>

void PhysicsComponent::OnInit(ActorManager *actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    collider = owner->GetComponent<ColliderComponent>();
}

void PhysicsComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessPhysics(dt);
    collider->GetCollider()->UpdatePosition(transform->position + collider->GetOffset());
    ProcessCollisionDetectionAndResolution();
}

void PhysicsComponent::OnLateUpdate(ActorManager* actorManager, float dt)
{
    // ProcessCollisionDetectionAndResolution();
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
    float tOut;
    grounded = CollisionWorld::Get()->Intersect(groundSegment, tOut, collider->GetId());

    // Collision detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();

    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    if(CollisionWorld::Get()->Intersect(*collider->GetCollider(), collisionData))
    {
        int iterations = 0;
        while(collisionData.size > 0 && iterations < 10)
        {
            Vector3 n = collisionData[0].n;
            f32 penetration = collisionData[0].penetration;
            transform->position += n * penetration;
            transform->position += n * 0.001f;
            acceleration = Vector3(0.0f);
            velocity -= n * velocity.Dot(n);

            collisionData.Clear();
            collider->GetCollider()->UpdatePosition(transform->position + collider->GetOffset());
            CollisionWorld::Get()->Intersect(*collider->GetCollider(), collisionData);
            iterations++;
        }    
    }

    MemoryManager::Get()->ReleaseFrame(frame);
}