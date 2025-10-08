#include "button_render_component.h"
#include "transform_component.h"
#include "button_component.h"

#include <math/matrix4.h>
#include <model.h>
#include <material.h>

#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>
#include <math/algebra.h>
#include <asset_managers/material_manager.h>

UniformBuffer * ButtonRenderComponent::uniformBuffer = nullptr;
ButtonRenderComponent::PerDrawUbo ButtonRenderComponent::ubo;

void ButtonRenderComponent::Initialize()
{
    ubo.model = Matrix4(1.0f);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &ubo, sizeof(ubo), 1);
}

void ButtonRenderComponent::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
}

void ButtonRenderComponent::OnSetRenderState()
{
    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    VertexShaderManager::Get()->Bind("default");
}

void ButtonRenderComponent::OnInit(ActorManager *actorManager)
{
    buttonComponent = owner->GetComponent<ButtonComponent>();
    transform = owner->GetComponent<TransformComponent>();
    buttonMaterial = static_cast<SolidColorMaterial*>(MaterialManager::Get()->Get("RedBorderMaterial"));
    buttonMaterial->SetAmbient(Vector3(5, 0, 0));
}

void ButtonRenderComponent::OnTerminate(ActorManager *actorManager)
{
}

void ButtonRenderComponent::OnRender(ActorManager *actorManager)
{
    if (buttonComponent->IsSignal())
    {
        buttonMaterial->SetAmbient(Vector3(0, 5, 0));
    }
    else
    {
        buttonMaterial->SetAmbient(Vector3(5, 0, 0));
    }

    Matrix4 tra = Matrix4::Translate(transform->position); 
    Matrix4 sca = Matrix4::Scale(transform->scale);
    Matrix4 ori = Matrix4::TransformFromEuler(transform->rotation.x, transform->rotation.y, transform->rotation.z);

    ubo.model = sca * ori * tra;
    Matrix4 rotOffset = Matrix4::RotateX(rotationOffset.x) * Matrix4::RotateY(rotationOffset.y) * Matrix4::RotateZ(rotationOffset.z);
    ubo.model = rotOffset * ubo.model;
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);    
    
    model->Draw();
}