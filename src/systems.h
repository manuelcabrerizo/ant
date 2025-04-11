#pragma once

class PlayerController
{
private:
     float speed;
     bool showMouse;
     vec3 moveDirection;

     InputComponent *input;
     TransformComponent *transform;
     CameraComponent *camera;
     PhysicsComponent *physics;

     void ProcessMouseMovement();
     void ProcessKeyboardMovement();
     void ProcessPhysics(float dt);
     void ProcessColisionDetectionAndResolution(CollisionWorld *cw);
public:
     void Init();
     void Terminate();
     void Update(ActorManager *am, CollisionWorld *cw, f32 dt);
};

struct CameraUbo
{
     mat4 view;
     mat4 proj;
};

class CameraSystem
{
private:
     UniformBuffer *uniformBuffer;
     CameraUbo ubo;
public:
     void Init();
     void Terminate();
     void Update(ActorManager *am, f32 dt);
};

struct PerDrawUbo
{
     mat4 model;
};

class RenderSystem
{
private:
     UniformBuffer *uniformBuffer;
     PerDrawUbo ubo;
public:
     void Init();
     void Terminate();
     void Render(ActorManager *am, f32 dt);
};

class WeaponSystem : public INotificable
{
public:
     void Init();
     void Terminate();
     void Update(ActorManager *am, f32 dt);

     void OnNotify(NotificationType type, Notification notification, void *sender) override;
};
