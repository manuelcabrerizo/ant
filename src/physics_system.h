#pragma once

class PhysicsSystem
{
private:
    TransformComponent *transform;
    PhysicsComponent *physics;

    void ProcessPhysics(float dt);
    void ProcessColisionDetectionAndResolution(CollisionWorld *cw);
public:
    void Init();
    void Terminate();
    void Update(ActorManager *actorManager, CollisionWorld *collisionWorld, f32 dt);
};