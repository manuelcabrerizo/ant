#pragma once

struct CameraUbo
{
     mat4 view;
     mat4 proj;
};

class CameraComponent : public Component<CameraComponent>, INotificable
{
private:
     // TODO: see if this realy needs to be static
     static UniformBuffer *uniformBuffer;
     static CameraUbo ubo;

     TransformComponent *transform;

     vec3 position;
     vec3 front;
     vec3 right;
     vec3 up;
     vec3 worldUp;
     vec3 worldFront;
public:
     // use to init static variables, must be called by the user at init time
     static void Initialize();
     static void Terminate();

     // Component logic interface, this are call form the ActorManager automaticaly
     void OnInit(ActorManager *actorManager);
     void OnTerminate(ActorManager *actorManager) override;
     void OnUpdate(ActorManager *actorManager, f32 dt);
     void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;

     // Camera custom functions
     void Init(vec3 pos, vec3 dir);
     void SetPosition(vec3 pos);
     void SetDirection(vec3 dir);
     vec3 GetPosition();
     vec3 GetFront();
     vec3 GetRight();
     vec3 GetUp();
     vec3 GetWorldFront();
     vec3 GetWorldUp();
     mat4 GetView();
};
