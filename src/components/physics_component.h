#pragma once

#include "component.h"
#include <math/vector3.h>

class TransformComponent;
class ColliderComponent;

class PhysicsComponent : public Component<PhysicsComponent>
{
private:
    void ProcessPhysics(float dt);
    void ProcessCollisionDetectionAndResolution();

public:
    TransformComponent *transform;
    ColliderComponent* collider;

    Vector3 acceleration = Vector3();
    Vector3 velocity = Vector3();
    Vector3 forceAccumulator = Vector3();
    bool grounded = false;

    void OnInit(ActorManager *actorManager);
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnLateUpdate(ActorManager* actorManager, float dt);
};