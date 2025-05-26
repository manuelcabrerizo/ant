#pragma once

#include "component.h"
#include <collision.h>

class TransformComponent;

class PhysicsComponent : public Component<PhysicsComponent>
{
private:
    static CollisionWorld collisionWorld;

    void ProcessPhysics(float dt);
    void ProcessColisionDetectionAndResolution();

public:
    TransformComponent *transform;

    Vector3 acceleration;
    Vector3 velocity;
    Vector3 forceAccumulator;
    Vector3 offset;
    bool grounded = false;

    static void Initialize();
    static void Terminate();

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
};