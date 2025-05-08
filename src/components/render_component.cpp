UniformBuffer *RenderComponent::uniformBuffer = nullptr;
PerDrawUbo RenderComponent::ubo;
UniformBuffer *RenderComponent::matrixBuffer = nullptr;
     
void RenderComponent::Initialize()
{
    ubo.model = Matrix4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    Matrix4 buffer[100] = {};
    matrixBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, buffer, sizeof(Matrix4)*100, 2);
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
    Matrix4 tra = Matrix4::Translate(transform->position); 
    Matrix4 sca = Matrix4::Scale(transform->scale);

    Vector3 front = transform->direction.Normalized();

    Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 right =  worldUp.Cross(front).Normalized();
    Vector3 up = front.Cross(right);
    Matrix4 ori = Matrix4::TransformFromBasis(Vector3(0.0f), right, up, front);
    ubo.model =  tra * ori * sca; // TODO: chage order of mul

    Matrix4 rotOffset = Matrix4::RotateX(rotationOffset.x) * Matrix4::RotateY(rotationOffset.y) * Matrix4::RotateZ(rotationOffset.z);

    ubo.model = ubo.model * rotOffset;

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