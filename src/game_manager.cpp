// TODO:
// - Implement a small demo with a full loop
// the player has to have life and be able to shoot at least two weapons
// the enemies have to move arrown the map and shoot the player
// when the player kills all enemies the game restart
// when the player gets kill the game restart

#include <game_manager.h>

#include <collision.h>

#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <asset_managers/material_manager.h>
#include <asset_managers/model_manager.h>

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

void GameManager::Init()
{
    InitializeAssetsManagers();

    CameraComponent::Initialize();
    RenderComponent::Initialize();
    CollisionWorld::Init(100);

    GraphicsManager::Get()->DebugInit();

    LoadDefaultAssets();
    InitializeActorManager();

    scenes.Init(1, STATIC_MEMORY);

    menuState.Init(this);
    gameState.Init(this);
    stateMachine.Push(&menuState);
}

void GameManager::Update(f32 dt)
{
    GraphicsManager::Get()->BeginFrame(0.2f, 0.2f, 0.4f);
    CollisionWorld::Get()->DebugDraw();
    stateMachine.Update(dt);
}

void GameManager::Render(f32 dt)
{
    stateMachine.Render();
    GraphicsManager::Get()->DebugPresent();
    GraphicsManager::Get()->EndFrame(1);
}

void GameManager::Terminate()
{
    stateMachine.Clear();
    actorManager.Terminate();

    GraphicsManager::Get()->DebugTerminate();

    CollisionWorld::Terminate();
    RenderComponent::Terminate();
    CameraComponent::Terminate();

    ShutdownAssetsManagers();
}

void GameManager::ChangeToMenuState()
{
    stateMachine.ChangeState(&menuState);
}

void GameManager::ChangeToGameState()
{
    stateMachine.ChangeState(&gameState);
}

ActorManager* GameManager::GetActorManager()
{
    return &actorManager;
}

Scene* GameManager::GetCurrentScene()
{
    return &scenes[0];
}

void GameManager::InitializeAssetsManagers()
{
    VertexShaderManager::Initialize(4);
    FragmentShaderManager::Initialize(4);
    TextureManager::Initialize(64);
    MaterialManager::Initialize(64);
    ModelManager::Initialize(32);
}

void GameManager::ShutdownAssetsManagers()
{
    MaterialManager::Shutdown();
    TextureManager::Shutdown();
    ModelManager::Shutdown();
    VertexShaderManager::Shutdown();
    FragmentShaderManager::Shutdown();
}

void GameManager::LoadDefaultAssets()
{
    // Load Vertex the shaders
    VertexShaderManager::Get()->Load("default", "data/shaders/vert.hlsl");
    VertexShaderManager::Get()->Load("animation", "data/shaders/animation_vert.hlsl");
    VertexShaderManager::Get()->Load("ui_vert", "data/shaders/ui_vert.hlsl");
    VertexShaderManager::Get()->Bind("default");

    // Load Fragment the shaders
    FragmentShaderManager::Get()->Load("default", "data/shaders/frag.hlsl");
    FragmentShaderManager::Get()->Load("color", "data/shaders/color.hlsl");
    FragmentShaderManager::Get()->Load("ui_frag", "data/shaders/ui_frag.hlsl");
    FragmentShaderManager::Get()->Bind("default");

    // Load textures
    TextureManager::Get()->Load("DefaultMaterial_Diffuse", "data/textures/DefaultTextures/DefaultMaterial_Diffuse.png");
    TextureManager::Get()->Load("DefaultMaterial_Normal", "data/textures/DefaultTextures/DefaultMaterial_Normal.png");
    TextureManager::Get()->Load("DefaultMaterial_Specular", "data/textures/DefaultTextures/DefaultMaterial_Specular.png");

    // Load Materials
    MaterialManager::Get()->LoadTexture("DefaultMaterial", "default",
        "DefaultMaterial_Diffuse", "DefaultMaterial_Normal", "DefaultMaterial_Specular", 64);
}

void GameManager::InitializeActorManager()
{
    actorManager.BeingInitialization(100, 64, STATIC_MEMORY);
    actorManager.AddComponentType<TransformComponent, 100>();
    actorManager.AddComponentType<RenderComponent, 100>();
    actorManager.AddComponentType<PhysicsComponent, 100>();
    actorManager.AddComponentType<ColliderComponent, 100>();
    actorManager.AddComponentType<PlayerControllerComponent, 1>();
    actorManager.AddComponentType<WeaponComponent, 100>();
    actorManager.AddComponentType<CameraComponent, 1>();
    actorManager.AddComponentType<EnemyComponent, 10>();
    actorManager.AddComponentType<AnchorComponent, 10>();
    actorManager.AddComponentType<AnimationComponent, 10>();
    actorManager.AddComponentType<BulletComponent, 100>();
    // NOTE: add more component types ...
    actorManager.EndInitialization();
}