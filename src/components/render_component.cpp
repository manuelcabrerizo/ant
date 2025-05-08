UniformBuffer *RenderComponent::uniformBuffer = nullptr;
PerDrawUbo RenderComponent::ubo;
UniformBuffer *RenderComponent::matrixBuffer = nullptr;
     
void RenderComponent::Initialize()
{
    ubo.model = mat4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    mat4 buffer[100] = {};
    matrixBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, buffer, sizeof(mat4)*100, 2);
    GraphicsManager::Get()->UniformBufferBind(matrixBuffer);
}

void RenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->UniformBufferFree(matrixBuffer);
}

void RenderComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
    if(isAnimated)
    {
        animation = actorManager->GetComponent<AnimationComponent>(owner);
    }
}

void RenderComponent::OnTerminate(ActorManager *actorManager)
{
}

void RenderComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    if(isAnimated && animation)
    {
        animation->skeleton.Animate(&animation->animation, dt*0.54f);
    }
}


void RenderComponent::OnRender(ShaderManager *shaderManager, ActorManager *actorManager)
{
    mat4 tra = translate(mat4(1.0f), transform->position); 
    mat4 sca = scale(mat4(1.0f), transform->scale);

    vec3 front = normalize(transform->direction);

    vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = normalize(cross(worldUp, front));
    vec3 up = cross(front, right);
    mat3 rot = mat3(right, up, front);
    mat4 ori = mat4(rot);
    ori[3][3] = 1.0f;
    ubo.model =  tra * ori * sca;

    if(!isAnimated)
    {
        shaderManager->Bind("default"); 
    }
    else
    {
        GraphicsManager::Get()->UniformBufferUpdate(matrixBuffer, animation->skeleton.GetMatrices());
        shaderManager->Bind("animation");
        // TODO: temporal fix becouse warrior animation are wrong
        //ubo.model *= rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    }

    GraphicsManager::Get()->TextureBind(texture, 0);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
    model->Draw();
}