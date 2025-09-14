#include "animated_render_component.h"
#include "transform_component.h"
#include "animation_component.h"

#include <math/matrix4.h>
#include <model.h>

#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>

UniformBuffer* AnimatedRenderComponent::uniformBuffer = nullptr;
AnimatedRenderComponent::PerDrawUbo AnimatedRenderComponent::ubo;
UniformBuffer* AnimatedRenderComponent::matrixBuffer = nullptr;

void AnimatedRenderComponent::Initialize()
{
    ubo.model = Matrix4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);

    Matrix4 buffer[100] = {};
    matrixBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, buffer, sizeof(Matrix4) * 100, 2);
    GraphicsManager::Get()->UniformBufferBind(matrixBuffer);
}

void AnimatedRenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->UniformBufferFree(matrixBuffer);
}

void AnimatedRenderComponent::OnSetRenderState()
{
    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    VertexShaderManager::Get()->Bind("animation");
}

void AnimatedRenderComponent::OnInit(ActorManager* actorManager)
{
    animation = owner->GetComponent<AnimationComponent>();
}

void AnimatedRenderComponent::OnTerminate(ActorManager* actorManager)
{
}

void AnimatedRenderComponent::OnRender(ActorManager* actorManager)
{
    TransformComponent* transform = owner->GetComponent<TransformComponent>();

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
    GraphicsManager::Get()->UniformBufferUpdate(matrixBuffer, animation->GetMatrices());
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);

    model->Draw();
}