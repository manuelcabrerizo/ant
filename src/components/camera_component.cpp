UniformBuffer *CameraComponent::uniformBuffer;
CameraUbo CameraComponent::ubo;

void CameraComponent::Initialize()
{
     ubo.view = lookAt(vec3(0.0f, 4.0f, -4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
     ubo.proj = perspective(radians(65.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.01f, 100.0f);
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
     vec2 *extend = (vec2 *)data;
     ubo.proj = perspective(radians(65.0f), extend->x/extend->y, 0.01f, 100.0f);
}

void CameraComponent::Init(vec3 pos, vec3 dir)
{
     worldUp = vec3(0.0f, 1.0f, 0.0f);
     
     position = pos;
     front = normalize(dir);
     right = normalize(cross(worldUp, front));
     up = cross(front, right);
     // TODO: this should never be zero
     worldFront = normalize(vec3(front.x, 0.0f, front.z));
}

void CameraComponent::SetPosition(vec3 pos)
{
     position = pos;
}

void CameraComponent::SetDirection(vec3 dir)
{
     front = normalize(dir);
     right = normalize(cross(worldUp, front));
     up = cross(front, right);
     // TODO: this should never be zero
     worldFront = normalize(vec3(front.x, 0.0f, front.z));
}

vec3 CameraComponent::GetPosition()
{
     return position;
}

vec3 CameraComponent::GetFront()
{
     return front;
}

vec3 CameraComponent::GetRight()
{
     return right;
}

vec3 CameraComponent::GetUp()
{
     return up;
}

vec3 CameraComponent::GetWorldFront()
{
     return worldFront;
}

vec3 CameraComponent::GetWorldUp()
{
     return worldUp;
}


mat4 CameraComponent::GetView()
{
     return lookAt(position, position + front, worldUp);
}
