#pragma once

struct CameraUbo
{
     mat4 view;
     mat4 proj;
};

class CameraSystem : public INotificable
{
private:
     UniformBuffer *uniformBuffer;
     CameraUbo ubo;

     void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
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

     void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
};
