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
     ubo.view = Matrix4::LookAt(Vector3(0.0f, 4.0f, 20.0f), Vector3(0.0f, 4.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
     ubo.proj = Matrix4::Perspective(65.0f, (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.01f, 100.0f);
     uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 0);
     GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}

void CameraComponent::Terminate()
{
     GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void CameraComponent::OnInit(ActorManager *actorManager)
{
     resizeNotification = NotificationManager::Get()->AddListener(this, NOTIFICATION_ON_RESIZE);
     transform = actorManager->GetComponent<TransformComponent>(owner);
}

void CameraComponent::OnTerminate(ActorManager *actorManager)
{
     NotificationManager::Get()->RemoveListener(resizeNotification, NOTIFICATION_ON_RESIZE);
}

void CameraComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
     SetPosition(transform->position);
     SetDirection(transform->direction);
     ubo.view = GetView();
     GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
}

void CameraComponent::OnNotify(NotificationType type, void *data, size_t size, void *sender)
{
     Vector2 *extend = (Vector2 *)data;
     ubo.proj = Matrix4::Perspective(65.0f, extend->x/extend->y, 0.01f, 100.0f);
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
