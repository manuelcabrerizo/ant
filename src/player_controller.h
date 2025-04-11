#pragma once

class PlayerController
{
private:
     float speed;
     bool showMouse;
     vec3 moveDirection;

     PlayerControllerComponent *playerController;
     TransformComponent *transform;
     CameraComponent *camera;
     PhysicsComponent *physics;

     void ProcessMouseMovement();
     void ProcessKeyboardMovement();
     void ProcessPhysics(float dt);
     void ProcessColisionDetectionAndResolution(CollisionWorld *cw);
     void ChangeWeapon(ActorManager *actorManager);

public:
     void Init();
     void Terminate();
     void Update(ActorManager *am, CollisionWorld *cw, f32 dt);
};