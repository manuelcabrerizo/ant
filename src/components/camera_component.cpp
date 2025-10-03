#include "camera_component.h"
#include "transform_component.h"

#include <math/vector2.h>
#include <math/matrix4.h>

#include <graphics_manager.h>
#include <notification_manager.h>

UniformBuffer *CameraComponent::uniformBuffer;
CameraUbo CameraComponent::ubo;

void CameraComponent::Initialize()
{
     uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 0);
}

void CameraComponent::Terminate()
{
     GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void CameraComponent::OnInit(ActorManager *actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::OnResize);

    int windowWidth, windowHeight;
    PlatformClientDimensions(&windowWidth, &windowHeight);
    ubo.proj = Matrix4::Perspective(90.0f, (float)windowWidth / (float)windowHeight, 0.05f, 200.0f);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    transform = owner->GetComponent<TransformComponent>();
}

void CameraComponent::OnTerminate(ActorManager *actorManager)
{
     NotificationManager::Get()->RemoveListener(this, NotificationType::OnResize);
}

void CameraComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
     SetPosition(transform->position);
     SetDirection(Matrix4::TransformVector(Matrix4::TransformFromEuler(transform->rotation), Vector3::forward));
     ubo.view = GetView();
     GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
     // TODO: try no to do this on the Update
     GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

     GraphicsManager::Get()->UpdateViewPosition(transform->position);
}

void CameraComponent::OnResize(OnResizeNotification* onResize)
{
    ubo.proj = Matrix4::Perspective(90.0f, onResize->extent.x / onResize->extent.y, 0.05f, 200.0f);
}

void CameraComponent::OnNotify(NotificationType type, Notification *notification)
{
    switch (type)
    {
        case NotificationType::OnResize: OnResize((OnResizeNotification*)notification); break;
    }
}

void CameraComponent::Init(Vector3 pos, Vector3 dir)
{
     worldUp = Vector3(0.0f, 1.0f, 0.0f);
     
     position = pos;
     front = dir.Normalized();
     right = worldUp.Cross(front).Normalized();
     up = front.Cross(right);
     // TODO: this should never be zero
     worldFront = Vector3(front.x, 0.0f, front.z).Normalized();
}

void CameraComponent::SetPosition(Vector3 pos)
{
     position = pos;
}

void CameraComponent::SetDirection(Vector3 dir)
{
     front = dir.Normalized();
     right = worldUp.Cross(front).Normalized();
     up = front.Cross(right);
     // TODO: this should never be zero
     worldFront = Vector3(front.x, 0.0f, front.z).Normalized();
}

Vector3 CameraComponent::GetPosition()
{
     return position;
}

Vector3 CameraComponent::GetFront()
{
     return front;
}

Vector3 CameraComponent::GetRight()
{
     return right;
}

Vector3 CameraComponent::GetUp()
{
     return up;
}

Vector3 CameraComponent::GetWorldFront()
{
     return worldFront;
}

Vector3 CameraComponent::GetWorldUp()
{
     return worldUp;
}


Matrix4 CameraComponent::GetView()
{
     return Matrix4::LookAt(position, position + front, worldUp);
}
