#include "physics_component.h"
#include "transform_component.h"

#include <math.h>

#include <memory_manager.h>

CollisionWorld PhysicsComponent::collisionWorld;

void PhysicsComponent::Initialize()
{
    collisionWorld.LoadFromFile("data/collision/level-collision.obj");
}

void PhysicsComponent::Terminate()
{

}

void PhysicsComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
}

void PhysicsComponent::OnTerminate(ActorManager *actorManager)
{

}

void PhysicsComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessPhysics(dt);
    ProcessColisionDetectionAndResolution();
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

void PhysicsComponent::ProcessColisionDetectionAndResolution()
{
    f32 colliderRadius = 0.5f;

    // Grounded test
    Segment groundSegment;
    Vector3 colliderPos = transform->position + offset;
    groundSegment.Init(colliderPos, colliderPos - Vector3(0.0f, colliderRadius + 0.05f, 0.0f));
    float tOut; Vector3 nOut;
    grounded = collisionWorld.Intersect(groundSegment, tOut, nOut);

    // Colission detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    Sphere sphere;
    sphere.Init(transform->position + offset, colliderRadius);
    if(collisionWorld.Intersect(sphere, collisionData))
    {
         while(collisionData.size > 0)
         {
            Vector3 n = collisionData[0].n;
              f32 penetration = collisionData[0].penetration;
              transform->position += n * penetration;
              transform->position += n * 0.001f;
              acceleration = Vector3(0.0f);
              velocity -= n * velocity.Dot(n);

              collisionData.Clear();

              sphere.Init(transform->position + offset, colliderRadius);
              collisionWorld.Intersect(sphere, collisionData);                    
         }    
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}