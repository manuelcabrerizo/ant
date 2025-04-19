#pragma once

class PhysicsSystem
{
private:
    TransformComponent *transform;
    PhysicsComponent *physics;
    CollisionWorld collisionWorld;

    void ProcessPhysics(float dt);
    void ProcessColisionDetectionAndResolution();
public:
    void Init();
    void Terminate();
    void Update(ActorManager *actorManager, f32 dt);
};