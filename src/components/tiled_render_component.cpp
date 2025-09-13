#include "tiled_render_component.h"
#include "transform_component.h"

#include <model.h>

#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>

UniformBuffer* TiledRenderComponent::uniformBuffer = nullptr;
TiledRenderComponent::PerDrawUbo TiledRenderComponent::ubo;

void TiledRenderComponent::Initialize()
{
    ubo.model = Matrix4(1.0f);
    ubo.width = 1.0f;
    ubo.height = 1.0f;
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
}

void TiledRenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void TiledRenderComponent::OnSetRenderState()
{
    GraphicsManager::Get()->SetRasterizerStateCullNone();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    VertexShaderManager::Get()->Bind("tiled_vert");
}

void TiledRenderComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
}

void TiledRenderComponent::OnTerminate(ActorManager* actorManager)
{
}

void TiledRenderComponent::OnRender(ActorManager* actorManager)
{
    Matrix4 tra = Matrix4::Translate(transform->position);
    Matrix4 sca = Matrix4::Scale(transform->scale);

    Vector3 front = transform->direction.Normalized();
    Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
    Vector3 up = Vector3::Cross(front, right);

    Matrix4 ori = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);

    GraphicsManager::Get()->DebugDrawLine(transform->position, transform->position + right, Vector3(1, 0, 0));
    GraphicsManager::Get()->DebugDrawLine(transform->position, transform->position + up, Vector3(0, 1, 0));
    GraphicsManager::Get()->DebugDrawLine(transform->position, transform->position + front, Vector3(0, 0, 1));

    ubo.model = sca * ori * tra;

    Matrix4 rotOffset = Matrix4::RotateX(rotationOffset.x) * Matrix4::RotateY(rotationOffset.y) * Matrix4::RotateZ(rotationOffset.z);

    ubo.model = rotOffset * ubo.model;
    ubo.width = transform->scale.x;
    ubo.height = transform->scale.y;

    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
    model->Draw();
}

void TiledRenderComponent::SetModel(Model* model)
{
    this->model = model;
}

Model* TiledRenderComponent::GetModel() const
{
    return model;
}
void TiledRenderComponent::SetRotationOffset(const Vector3& offset)
{
    this->rotationOffset = offset;
}

Vector3 TiledRenderComponent::GetRotationOffset()
{
    return rotationOffset;
}