void CameraSystem::Init()
{
     ubo.view = lookAt(vec3(0.0f, 4.0f, -4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
     ubo.proj = perspective(radians(60.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.01f, 100.0f);
     uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 0);
     GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}

void CameraSystem::Terminate()
{
     GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void CameraSystem::Update(ActorManager *am, f32 dt)
{
     Array<CameraComponent> *cameras = am->GetCameraComponents();
     for(u32 i = 0; i < cameras->size; ++i)
     {
          SlotmapKey<Actor> actor = (*cameras)[i].owner;
          CameraComponent *camera = &(*cameras)[i];
          TransformComponent *transform = am->GetTransformComponent(actor);

          camera->SetPosition(transform->position/* + camera->GetWorldUp() * 2.0f - camera->GetWorldFront() * 2.0f*/);
          camera->SetDirection(transform->direction);
          ubo.view = camera->GetView();
          GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
     }
}
