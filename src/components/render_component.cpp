UniformBuffer *RenderComponent::uniformBuffer = nullptr;
PerDrawUbo RenderComponent::ubo;

UniformBuffer *RenderComponent::matrixBuffer;
Skeleton RenderComponent::skeleton;
Animation RenderComponent::animation;
     
void RenderComponent::Initialize()
{
    ubo.model = mat4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);


    skeleton.Init("../data/models/warrior.dae", STATIC_MEMORY);
    animation.Init("../data/animations/walk_front.dae", STATIC_MEMORY);
    matrixBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, skeleton.GetMatrices(), sizeof(mat4)*100, 2);
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
}

void RenderComponent::OnTerminate(ActorManager *actorManager)
{
}

void RenderComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    if(isAnimated)
    {
        skeleton.Animate(&animation, dt*0.25f);
    }
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

    GraphicsManager::Get()->TextureBind(texture, 0);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
    if(!isAnimated)
    {
        shaderManager->Bind("default"); 
    }
    else
    {
        GraphicsManager::Get()->UniformBufferUpdate(matrixBuffer, skeleton.GetMatrices());
        shaderManager->Bind("animation");
    }
    model->Draw();
}