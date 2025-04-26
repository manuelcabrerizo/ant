UniformBuffer *RenderComponent::uniformBuffer = nullptr;
PerDrawUbo RenderComponent::ubo;
     
void RenderComponent::Initialize()
{
    ubo.model = mat4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}

void RenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void RenderComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
}

void RenderComponent::OnTerminate(ActorManager *actorManager)
{

}

void RenderComponent::OnRender(ShaderManager *shaderManager, ActorManager *actorManager)
{
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
    shaderManager->Bind("default"); 
    GraphicsManager::Get()->TextureBind(texture, 0);
    model->Draw();
}