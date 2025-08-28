#include "play_state.h"
#include <game_manager.h>
#include <input_manager.h>

#include <asset_managers/texture_manager.h>

#include <components/component.h>
#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/physics_component.h>
#include <components/collider_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/anchor_component.h>
#include <components/player_controller_component.h>
#include <components/enemy_component.h>
#include <components/animation_component.h>
#include <components/bullet_component.h>
#include <collision.h>
#include <asset_managers/shader_manager.h>
#include <math/algebra.h>

void PlayState::Init(GameManager *gameManager)
{
    uiRenderer.Init();
    this->gameManager = gameManager;
    this->scene = gameManager->GetCurrentScene();

    TextureManager::Get()->Load("Blood", "data/textures/blood.png");
    batchRenderer = GraphicsManager::Get()->BatchRendererAlloc(
        VertexShaderManager::Get()->Get("particle_vert"),
        FragmentShaderManager::Get()->Get("particle_frag"),
        TextureManager::Get()->Get("Blood"));
}

void PlayState::Terminate()
{
    uiRenderer.Terminate();
    GraphicsManager::Get()->BatchRendererFree(batchRenderer);
    TextureManager::Get()->Unload("Blood");

    batchRenderer = nullptr;
}

void PlayState::OnEnter()
{
    memoryFrame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);

    NotificationManager::Get()->AddListener(this, NotificationType::OnResize);

    PlatformClientDimensions(&windowWidth, &windowHeight);
    Vector2 extent = Vector2(windowWidth, windowHeight);
    OnResizeNotification notification;
    notification.extent = extent;
    OnResize(&notification);

    TextureManager::Get()->Load("Crosshair", "data/textures/Crosshair.png");

    crosshairSize = Vector2(32, 32);
    crosshairPosition = (extent * 0.5f);

    CameraComponent::Initialize();
    RenderComponent::Initialize();

    InitializeActorManager();

    scene->Load(&actorManager, "");
}

void PlayState::OnExit()
{
    scene->Unload();

    actorManager.Terminate();

    RenderComponent::Terminate();
    CameraComponent::Terminate();

    TextureManager::Get()->Unload("Crosshair");

    NotificationManager::Get()->RemoveListener(this, NotificationType::OnResize);

    MemoryManager::Get()->ReleaseFrame(memoryFrame);
}

void PlayState::OnUpdate(float deltaTime)
{
    // Initialize new components
    actorManager.InitializeNewComponents();
    // Update
    actorManager.UpdateComponents<PlayerControllerComponent>(deltaTime);
    actorManager.UpdateComponents<CameraComponent>(deltaTime);
    actorManager.UpdateComponents<WeaponComponent>(deltaTime);
    actorManager.UpdateComponents<EnemyComponent>(deltaTime);
    actorManager.UpdateComponents<PhysicsComponent>(deltaTime);
    actorManager.UpdateComponents<ColliderComponent>(deltaTime);
    actorManager.UpdateComponents<RenderComponent>(deltaTime);
    actorManager.UpdateComponents<BulletComponent>(deltaTime);
    // Late Update
    actorManager.LateUpdateComponents<PhysicsComponent>(deltaTime);

    actorManager.ProcessActorsToRemove();

    if (InputManager::Get()->KeyJustDown(KEY_P))
    {
        gameManager->ChangeToMenuState();
    }
}

void PlayState::OnRender()
{
    actorManager.RenderComponents<RenderComponent>();
    //CollisionWorld::Get()->DebugDraw();
    GraphicsManager::Get()->DebugPresent();

    // BatchRenderer Test
    for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
        {
            batchRenderer->DrawQuad(Vector3(x*0.2 - 3, y*0.2, 0),
                Vector3(.2f, .2f, 1.0f), ANT_PI * 0.25f, Vector3(1, 1, 1));
        }
    }

    batchRenderer->Present();

    // Render the Crosshair
    uiRenderer.DrawQuat(crosshairPosition, crosshairSize, 0, "Crosshair");
}

void PlayState::OnResize(OnResizeNotification* onResize)
{
    windowWidth = onResize->extent.x;
    windowHeight = onResize->extent.y;
    Vector2 extent = onResize->extent;
    crosshairPosition = (extent * 0.5f);
    uiRenderer.OnResize(extent);
}

void PlayState::InitializeActorManager()
{
    actorManager.BeingInitialization(128, 64, FRAME_MEMORY);
    actorManager.AddComponentType<TransformComponent, 100>();
    actorManager.AddComponentType<RenderComponent, 100>();
    actorManager.AddComponentType<PhysicsComponent, 100>();
    actorManager.AddComponentType<ColliderComponent, 100>();
    actorManager.AddComponentType<PlayerControllerComponent, 1>();
    actorManager.AddComponentType<WeaponComponent, 100>();
    actorManager.AddComponentType<CameraComponent, 1>();
    actorManager.AddComponentType<EnemyComponent, 40>();
    actorManager.AddComponentType<AnchorComponent, 10>();
    actorManager.AddComponentType<AnimationComponent, 40>();
    actorManager.AddComponentType<BulletComponent, 100>();
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