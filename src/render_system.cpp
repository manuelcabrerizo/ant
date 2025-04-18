void RenderSystem::Init()
{
     ubo.model = mat4(1.0f);
     uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
     GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}

void RenderSystem::Terminate()
{
     GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void RenderSystem::Render(ActorManager *am, f32 dt)
{     
     Array<RenderComponent>& renders = am->GetComponents<RenderComponent>();
     for(u32 i = 0; i < renders.size; ++i)
     {
          SlotmapKey<Actor> actor = renders[i].owner;
          RenderComponent *render = &renders[i];

          if(!render->enable)
          {
               continue;
          }

          TransformComponent *transform = am->GetComponent<TransformComponent>(actor);

          mat4 tra = translate(mat4(1.0f), transform->position); 
          mat4 sca = scale(mat4(1.0f), transform->scale);

          vec3 front = normalize(transform->direction);
          vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f), front));
          vec3 up = cross(front, right);
          mat3 rot = mat3(right, up, front);
          mat4 ori = mat4(rot);
          ori[3][3] = 1.0f;
          ubo.model =  tra * ori * sca;
          GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
          GraphicsManager::Get()->TextureBind(render->texture, 0);
          render->model->Draw();
     }
}

     
