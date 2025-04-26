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
     void ChangeWeapon(ActorManager *actorManager);

public:
     void Init();
     void Terminate();
     void Update(ActorManager *am, f32 dt);
     void LateUpdate(ActorManager *actorManager, f32 dt);
};