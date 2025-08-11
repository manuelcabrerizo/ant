#include "physics_component.h"
#include "transform_component.h"
#include <math.h>
#include <memory_manager.h>
#include <collision.h>
#include <math/algebra.h>

#include <windows.h>

CollisionWorld PhysicsComponent::collisionWorld;

void PhysicsComponent::Initialize()
{
    collisionWorld.Initialize(50);

    Matrix4 rot = Matrix4::RotateY(ANT_PI / 4);
    Vector3 orientation[] =
    {
        Vector3(rot[0][0], rot[1][0], rot[2][0]),
        Vector3(rot[0][1], rot[1][1], rot[2][1]),
        Vector3(rot[0][2], rot[1][2], rot[2][2])
    };

    Vector3 orientation2[] =
    {
        Vector3(1, 0, 0),
        Vector3(0, 1, 0),
        Vector3(0, 0, 1)
    };

    // Enviroment Colliders Creation
    MeshCollider meshCollider;
    meshCollider.InitFromFile("data/collision/level-collision.obj");
    collisionWorld.AddCollider(Collider(meshCollider));

    OBB floor;
    floor.Init(Vector3(0, -0.5, 0), orientation2, Vector3(25, 0.5, 25));
    collisionWorld.AddCollider(Collider(floor));

    OBB obb;
    obb.Init(Vector3(6, 1.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddCollider(Collider(obb));

    OBB obb1;
    obb1.Init(Vector3(8, 2.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddCollider(Collider(obb1));

    OBB obb2;
    obb2.Init(Vector3(10, 3.5, 3), orientation, Vector3(1, 0.5f, 1));
    collisionWorld.AddCollider(Collider(obb2));
        
    OBB obb3;
    obb3.Init(Vector3(-3, 2.25, 10), orientation2, Vector3(2, 1, 2));
    collisionWorld.AddCollider(Collider(obb3));

    OBB obb4;
    obb4.Init(Vector3(-3, 1.5, 7.5), orientation2, Vector3(1, 0.5, 1));
    collisionWorld.AddCollider(Collider(obb4));

    Sphere sphe;
    sphe.Init(Vector3(0, 1.5, 7.5), 1.0f);
    collisionWorld.AddCollider(Collider(sphe));

    Capsule cap;
    cap.Init(Vector3(3, 1.5, 7.5), Vector3(3, 2.0f, 7.5), 0.25f);
    collisionWorld.AddCollider(Collider(cap));
}

void PhysicsComponent::Terminate()
{

}

void PhysicsComponent::DebugDraw()
{
    collisionWorld.DebugDraw();
}

void PhysicsComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);

    // Initialize our collider
    Matrix4 rot = Matrix4::RotateY(0);
    Vector3 orientation[] =
    {
        Vector3(rot[0][0], rot[1][0], rot[2][0]),
        Vector3(rot[0][1], rot[1][1], rot[2][1]),
        Vector3(rot[0][2], rot[1][2], rot[2][2])
    };

    Vector3 extent = Vector3(0.25, 0.75, 0.25);

    AABB aabb;
    aabb.Init(transform->position - extent, transform->position + extent);

    OBB obb;
    obb.Init(transform->position + offset, orientation, extent);

    float colliderRadius = 0.25f;
    Vector3 halfHeight = Vector3(0, 0.5, 0);
    Capsule capsule;
    capsule.Init(
        (transform->position - halfHeight) + offset,
        (transform->position + halfHeight) + offset,
        colliderRadius);

    Sphere sphere;
    sphere.Init(transform->position + offset, colliderRadius);

    collider = collisionWorld.AddCollider(Collider(capsule));
}

void PhysicsComponent::OnTerminate(ActorManager *actorManager)
{

}

void PhysicsComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessPhysics(dt);
    UpdateCollider();
    ProcessCollisionDetectionAndResolution();
}

void PhysicsComponent::UpdateCollider()
{
    collider->UpdatePosition(transform->position + offset);
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
    Vector3 colliderPos = transform->position + offset;
    groundSegment.Init(colliderPos, colliderPos - Vector3(0.0f, colliderRadius + 0.80f, 0.0f));
    float tOut;

    grounded = collisionWorld.Intersect(groundSegment, tOut, collider->GetId());
    
    // Collision detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();

    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    if(collisionWorld.Intersect(*collider, collisionData))
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
            collider->UpdatePosition(transform->position + offset);
            collisionWorld.Intersect(*collider, collisionData);
            iterations++;
        }    
    }

    MemoryManager::Get()->ReleaseFrame(frame);
}