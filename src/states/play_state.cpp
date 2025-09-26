#include "play_state.h"
#include <game_manager.h>
#include <input_manager.h>

#include <components/component.h>
#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/animated_render_component.h>
#include <components/tiled_render_component.h>
#include <components/physics_component.h>
#include <components/collider_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/anchor_component.h>
#include <components/player_controller_component.h>
#include <components/enemy_component.h>
#include <components/animation_component.h>
#include <components/portal_component.h>
#include <components/effect_component.h>
#include <components/key_component.h>
#include <components/fence_component.h>
#include <components/button_component.h>

#include <collision.h>
#include <math/algebra.h>

#include <asset_managers/shader_manager.h>
#include <asset_managers/model_manager.h>
#include <asset_managers/animation_manager.h>
#include <asset_managers/material_manager.h>
#include <asset_managers/texture_manager.h>

void PlayState::Init(GameManager *gameManager)
{
    uiRenderer.Init();
    textRenderer.Init("data/fonts/times.rtpa", STATIC_MEMORY);
    this->gameManager = gameManager;
    this->scene = gameManager->GetCurrentScene();
}

void PlayState::Terminate()
{
    uiRenderer.Terminate();
    textRenderer.Terminate();
}

void PlayState::OnEnter()
{
    this->timeAccumulator = 0;
    this->frameCounter = 0;
    this->FPS = 0;

    memoryFrame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);

    NotificationManager::Get()->AddListener(this, NotificationType::OnResize);

    PlatformClientDimensions(&windowWidth, &windowHeight);
    Vector2 extent = Vector2(windowWidth, windowHeight);
    OnResizeNotification notification;
    notification.extent = extent;
    OnResize(&notification);

    TextureManager::Get()->Load("Crosshair", "data/textures/Crosshair.png", FRAME_MEMORY);
    crosshairSize = Vector2(32, 32);
    crosshairPosition = (extent * 0.5f);

    CameraComponent::Initialize();
    RenderComponent::Initialize();
    AnimatedRenderComponent::Initialize();
    TiledRenderComponent::Initialize();

    InitializeActorManager();

    scene->Load(&actorManager, "");
}

void PlayState::OnExit()
{
    scene->Unload();

    actorManager.Terminate();

    TiledRenderComponent::Terminate();
    AnimatedRenderComponent::Terminate();
    RenderComponent::Terminate();
    CameraComponent::Terminate();

    NotificationManager::Get()->RemoveListener(this, NotificationType::OnResize);

    // Remove all assets loaded for the level
    AnimationManager::Get()->Clear(FRAME_MEMORY);
    SkeletonManager::Get()->Clear(FRAME_MEMORY);
    MaterialManager::Get()->Clear(FRAME_MEMORY);
    ModelManager::Get()->Clear(FRAME_MEMORY);
    VertexShaderManager::Get()->Clear(FRAME_MEMORY);
    FragmentShaderManager::Get()->Clear(FRAME_MEMORY);
    TextureManager::Get()->Clear(FRAME_MEMORY);
    
    MemoryManager::Get()->ReleaseFrame(memoryFrame);
}

void PlayState::OnUpdate(float deltaTime)
{
    frameCounter++;
    timeAccumulator += deltaTime;
    if (timeAccumulator >= 1.0f)
    {
        FPS = frameCounter;
        frameCounter = 0;
        timeAccumulator -= 1.0f;
    }

    // Initialize new components
    actorManager.InitializeNewComponents();
    // Update
    actorManager.UpdateComponents<PlayerControllerComponent>(deltaTime);
    actorManager.UpdateComponents<CameraComponent>(deltaTime);
    actorManager.UpdateComponents<WeaponComponent>(deltaTime);
    actorManager.UpdateComponents<EnemyComponent>(deltaTime);
    actorManager.UpdateComponents<PhysicsComponent>(deltaTime);
    actorManager.UpdateComponents<ColliderComponent>(deltaTime);
    actorManager.UpdateComponents<AnimationComponent>(deltaTime);
    actorManager.UpdateComponents<EffectComponent>(deltaTime);
    actorManager.UpdateComponents<KeyComponent>(deltaTime);
    actorManager.UpdateComponents<FenceComponent>(deltaTime);
    // Late Update
    actorManager.LateUpdateComponents<PhysicsComponent>(deltaTime);

    if (InputManager::Get()->KeyJustDown(KEY_P))
    {
        gameManager->ChangeToMenuState();
    }
}

void PlayState::OnRender()
{
    actorManager.RenderComponents<RenderComponent>();
    actorManager.RenderComponents<AnimatedRenderComponent>();
    actorManager.RenderComponents<TiledRenderComponent>();

    actorManager.RenderComponents<WeaponComponent>();
    actorManager.RenderComponents<EffectComponent>();

    CollisionWorld::Get()->DebugDraw();
    GraphicsManager::Get()->DebugPresent();

    // Render the Crosshair
    GraphicsManager::Get()->SetBlendingOff();
    GraphicsManager::Get()->SetDepthStencilOn();
    uiRenderer.DrawQuat(crosshairPosition, crosshairSize, 0, "Crosshair");

    // Render Text (the Render state is for rendering 3D text)
    GraphicsManager::Get()->SetAlphaBlending();
    GraphicsManager::Get()->SetDepthStencilOnWriteMaskZero();
    GraphicsManager::Get()->SetRasterizerStateCullNone();

    char buffer[256];
    sprintf(buffer, "FPS: %d", FPS);

    textRenderer.DrawString(buffer, Vector2(5, windowHeight - 32), 1.0f);
    textRenderer.DrawString("Hello, Sailor!", Vector2(5, windowHeight - 64), 1.0f);
    GraphicsManager::Get()->SetRasterizerStateCullBack();
    GraphicsManager::Get()->SetDepthStencilOn();

}

void PlayState::OnResize(OnResizeNotification* onResize)
{
    windowWidth = onResize->extent.x;
    windowHeight = onResize->extent.y;
    Vector2 extent = onResize->extent;
    crosshairPosition = (extent * 0.5f);
    uiRenderer.OnResize(extent);
    textRenderer.OnResize(extent);
}

void PlayState::InitializeActorManager()
{
    actorManager.BeingInitialization(128, 64, FRAME_MEMORY);
    actorManager.AddComponentType<TransformComponent, 100>();
    actorManager.AddComponentType<RenderComponent, 100>();
    actorManager.AddComponentType<AnimatedRenderComponent, 100>();
    actorManager.AddComponentType<TiledRenderComponent, 50>();
    actorManager.AddComponentType<PhysicsComponent, 100>();
    actorManager.AddComponentType<ColliderComponent, 100>();
    actorManager.AddComponentType<PlayerControllerComponent, 1>();
    actorManager.AddComponentType<WeaponComponent, 100>();
    actorManager.AddComponentType<CameraComponent, 1>();
    actorManager.AddComponentType<EnemyComponent, 40>();
    actorManager.AddComponentType<AnchorComponent, 10>();
    actorManager.AddComponentType<AnimationComponent, 40>();
    actorManager.AddComponentType<PortalComponent, 10>();
    actorManager.AddComponentType<EffectComponent, 10>();
    actorManager.AddComponentType<KeyComponent, 2>();
    actorManager.AddComponentType<FenceComponent, 10>();
    actorManager.AddComponentType<ButtonComponent, 10>();
    // NOTE: add more component types ...
    actorManager.EndInitialization();
}

void PlayState::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::OnResize: OnResize((OnResizeNotification*)notification); break;
    }
}