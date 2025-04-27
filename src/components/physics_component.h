#pragma once

class PhysicsComponent : public Component<PhysicsComponent>
{
private:
    static CollisionWorld collisionWorld;

    void ProcessPhysics(float dt);
    void ProcessColisionDetectionAndResolution();

public:
    TransformComponent *transform;

    vec3 acceleration;
    vec3 velocity;
    vec3 forceAccumulator;
    vec3 lastFrameAcceleration;
    vec3 offset;
    bool grounded = false;

    static void Initialize();
    static void Terminate();

    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
};