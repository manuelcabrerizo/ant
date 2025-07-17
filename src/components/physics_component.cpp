#include "physics_component.h"
#include "transform_component.h"

#include <math.h>

#include <memory_manager.h>

#include <collisions/obb.h>
#include <math/algebra.h>

CollisionWorld PhysicsComponent::collisionWorld;

void PhysicsComponent::Initialize()
{
    collisionWorld.LoadFromFile("data/collision/level-collision.obj");

    Matrix4 rot = Matrix4::RotateY(ANT_PI/4);
    Vector3 orientation[] =
    {
        Vector3(rot[0][0], rot[1][0], rot[2][0]),
        Vector3(rot[0][1], rot[1][1], rot[2][1]),
        Vector3(rot[0][2], rot[1][2], rot[2][2])
    };

    OBB obb;
    obb.Init(Vector3(6, 1.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddOBB(obb);

    OBB obb1;
    obb1.Init(Vector3(8, 2.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddOBB(obb1);

    OBB obb2;
    obb2.Init(Vector3(10, 3.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddOBB(obb2);

    Vector3 orientation2[] =
    {
        Vector3(1, 0, 0),
        Vector3(0, 1, 0),
        Vector3(0, 0, 1)
    };

    OBB obb3;
    obb3.Init(Vector3(-3, 2.25, 10), orientation2, Vector3(2, 1, 2));
    collisionWorld.AddOBB(obb3);

    OBB obb4;
    obb4.Init(Vector3(-3, 1.5, 7.5), orientation2, Vector3(1, 0.5, 1));
    collisionWorld.AddOBB(obb4);

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
    f32 colliderRadius = 0.25f;

    // Grounded test
    Segment groundSegment;
    Vector3 colliderPos = transform->position + offset;
    groundSegment.Init(colliderPos, colliderPos - Vector3(0.0f, colliderRadius + 0.80f, 0.0f));
    float tOut; Vector3 nOut;
    grounded = collisionWorld.Intersect(groundSegment, tOut, nOut);

    // Colission detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    Vector3 halfHeight = Vector3(0, 0.5, 0);
    Capsule capsule;
    capsule.Init(
        (transform->position - halfHeight) + offset,
        (transform->position + halfHeight) + offset,
        colliderRadius);

    /*
    Sphere sphere;
    sphere.Init(transform->position + offset, colliderRadius);
    */

    if(collisionWorld.Intersect(capsule, collisionData))
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

            //sphere.Init(transform->position + offset, colliderRadius);
            capsule.Init(
                (transform->position - halfHeight) + offset,
                (transform->position + halfHeight) + offset,
                colliderRadius);
            collisionWorld.Intersect(capsule, collisionData);
            iterations++;
        }    
    }

    //capsule.DebugDraw(16, Vector3(0, 1, 0));

    MemoryManager::Get()->ReleaseFrame(frame);
}