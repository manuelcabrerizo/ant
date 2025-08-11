#pragma once

#include "component.h"
#include <math/vector3.h>
#include <math/matrix4.h>
#include <notification_manager.h>

struct UniformBuffer;
class TransformComponent;

struct CameraUbo
{
     Matrix4 view;
     Matrix4 proj;
};

class CameraComponent : public Component<CameraComponent>, INotificable
{
private:
     // TODO: see if this realy needs to be static
     static UniformBuffer *uniformBuffer;
     static CameraUbo ubo;

     TransformComponent *transform;
     Vector3 position;
     Vector3 front;
     Vector3 right;
     Vector3 up;
     Vector3 worldUp;
     Vector3 worldFront;
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
     void Init(Vector3 pos, Vector3 dir);
     void SetPosition(Vector3 pos);
     void SetDirection(Vector3 dir);
     Vector3 GetPosition();
     Vector3 GetFront();
     Vector3 GetRight();
     Vector3 GetUp();
     Vector3 GetWorldFront();
     Vector3 GetWorldUp();
     Matrix4 GetView();
};
