#include <game_manager.h>
#include <collision.h>
#include <particle_system.h>
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <asset_managers/material_manager.h>
#include <asset_managers/model_manager.h>

void GameManager::Init()
{
    InitializeAssetsManagers();
    LoadDefaultAssets();
    CollisionWorld::Init(100);
    ParticleSystem::Init();
    GraphicsManager::Get()->DebugInit();
    scenes.Init(1, STATIC_MEMORY);
    menuState.Init(this);
    playState.Init(this);
    stateMachine.Push(&menuState);
}

void GameManager::Terminate()
{
    stateMachine.Clear();
    playState.Terminate();
    menuState.Terminate();
    GraphicsManager::Get()->DebugTerminate();
    ParticleSystem::Terminate();
    CollisionWorld::Terminate();
    ShutdownAssetsManagers();
}

void GameManager::Update(f32 dt)
{
    stateMachine.Update(dt);
}

void GameManager::Render(f32 dt)
{
    GraphicsManager::Get()->BeginFrame(0.2f, 0.2f, 0.4f);

    stateMachine.Render();

    GraphicsManager::Get()->EndFrame(1);
}

void GameManager::ChangeToMenuState()
{
    stateMachine.ChangeState(&menuState);
}

void GameManager::ChangeToPlayState()
{
    stateMachine.ChangeState(&playState);
}

Scene* GameManager::GetCurrentScene()
{
    return &scenes[0];
}

void GameManager::InitializeAssetsManagers()
{
    VertexShaderManager::Initialize(8);
    FragmentShaderManager::Initialize(8);
    TextureManager::Initialize(64);
    MaterialManager::Initialize(64);
    ModelManager::Initialize(32);
}

void GameManager::ShutdownAssetsManagers()
{
    ModelManager::Shutdown();
    MaterialManager::Shutdown();
    TextureManager::Shutdown();
    VertexShaderManager::Shutdown();
    FragmentShaderManager::Shutdown();
}

void GameManager::LoadDefaultAssets()
{
    // Load Vertex the shaders
    VertexShaderManager::Get()->Load("default", "data/shaders/vert.hlsl");
    VertexShaderManager::Get()->Load("animation", "data/shaders/animation_vert.hlsl");
    VertexShaderManager::Get()->Load("ui_vert", "data/shaders/ui_vert.hlsl");
    VertexShaderManager::Get()->Load("particle_vert", "data/shaders/particle_vert.hlsl");
    VertexShaderManager::Get()->Bind("default");

    // Load Fragment the shaders
    FragmentShaderManager::Get()->Load("default", "data/shaders/frag.hlsl");
    FragmentShaderManager::Get()->Load("color", "data/shaders/color.hlsl");
    FragmentShaderManager::Get()->Load("ui_frag", "data/shaders/ui_frag.hlsl");
    FragmentShaderManager::Get()->Load("particle_frag", "data/shaders/particle_frag.hlsl");
    FragmentShaderManager::Get()->Bind("default");

    // Load textures
    TextureManager::Get()->Load("DefaultMaterial_Diffuse", "data/textures/DefaultTextures/DefaultMaterial_Diffuse.png");
    TextureManager::Get()->Load("DefaultMaterial_Normal", "data/textures/DefaultTextures/DefaultMaterial_Normal.png");
    TextureManager::Get()->Load("DefaultMaterial_Specular", "data/textures/DefaultTextures/DefaultMaterial_Specular.png");

    // Load Materials
    MaterialManager::Get()->LoadTexture("DefaultMaterial", "default",
        "DefaultMaterial_Diffuse", "DefaultMaterial_Normal", "DefaultMaterial_Specular", 64);
}