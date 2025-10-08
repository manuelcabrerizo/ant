#include "weapon_render_component.h"
#include "transform_component.h"

#include <math/matrix4.h>
#include <model.h>

#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>
#include <math/algebra.h>

UniformBuffer* WeaponRenderComponent::uniformBuffer = nullptr;
WeaponRenderComponent::PerDrawUbo WeaponRenderComponent::ubo;

void WeaponRenderComponent::Initialize()
{
    ubo.model = Matrix4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
}

void WeaponRenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void WeaponRenderComponent::OnSetRenderState()
{
    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    VertexShaderManager::Get()->Bind("default");
}

void WeaponRenderComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
}

void WeaponRenderComponent::OnTerminate(ActorManager* actorManager)
{
}

void WeaponRenderComponent::OnRender(ActorManager* actorManager)
{
    Matrix4 tra = Matrix4::Translate(transform->position);
    Matrix4 sca = Matrix4::Scale(transform->scale);
    Matrix4 ori = Matrix4::TransformFromEuler(transform->rotation.x, transform->rotation.y, transform->rotation.z);

    ubo.model = sca * ori * tra;
    Matrix4 rotOffset = Matrix4::RotateX(rotationOffset.x) * Matrix4::RotateY(rotationOffset.y) * Matrix4::RotateZ(rotationOffset.z);
    ubo.model = rotOffset * ubo.model;
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);

    model->Draw();
}