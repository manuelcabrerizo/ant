#include <game_manager.h>
#include <collision.h>
#include <particle_systems/particle_system.h>
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <asset_managers/material_manager.h>
#include <asset_managers/model_manager.h>
#include <asset_managers/animation_manager.h>

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
    // TODO: automate this calls
    MaterialManager::Get()->Get("AlienPortalMaterial")->Update(dt);
    MaterialManager::Get()->Get("Level1MagmaMaterial")->Update(dt);

    stateMachine.Update(dt);

    /*
    size_t freeMemory = MemoryManager::Get()->GetFreeMemoryCount(STATIC_MEMORY);
    char buffer[256] = {};
    sprintf(buffer, "STATIC: free memory: %zu\n", freeMemory);
    OutputDebugString(buffer);

    freeMemory = MemoryManager::Get()->GetFreeMemoryCount(FRAME_MEMORY);
    sprintf(buffer, "FRAME: free memory: %zu\n", freeMemory);
    OutputDebugString(buffer);
    */
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
    SkeletonManager::Initialize(32);
    AnimationManager::Initialize(64);
}

void GameManager::ShutdownAssetsManagers()
{
    AnimationManager::Shutdown();
    SkeletonManager::Shutdown();
    ModelManager::Shutdown();
    MaterialManager::Shutdown();
    TextureManager::Shutdown();
    VertexShaderManager::Shutdown();
    FragmentShaderManager::Shutdown();
}

void GameManager::LoadDefaultAssets()
{
    // Load Vertex the shaders
    VertexShaderManager::Get()->Load("default", "data/shaders/vert.hlsl", STATIC_MEMORY);
    VertexShaderManager::Get()->Load("animation", "data/shaders/animation_vert.hlsl", STATIC_MEMORY);
    VertexShaderManager::Get()->Load("tiled_vert", "data/shaders/tiled_vert.hlsl", STATIC_MEMORY);
    VertexShaderManager::Get()->Load("ui_vert", "data/shaders/ui_vert.hlsl", STATIC_MEMORY);
    VertexShaderManager::Get()->Load("particle_vert", "data/shaders/particle_vert.hlsl", STATIC_MEMORY);
    VertexShaderManager::Get()->Bind("default");

    // Load Fragment the shaders
    FragmentShaderManager::Get()->Load("default", "data/shaders/frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("color", "data/shaders/color.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("ui_frag", "data/shaders/ui_frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("particle_frag", "data/shaders/particle_frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("portal_frag", "data/shaders/portal_frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("magma_frag", "data/shaders/magma_frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Load("fence_frag", "data/shaders/fence_frag.hlsl", STATIC_MEMORY);
    FragmentShaderManager::Get()->Bind("default");

    // Load textures
    TextureManager::Get()->Load("DefaultMaterial_Diffuse", "data/textures/DefaultTextures/DefaultMaterial_Diffuse.png", STATIC_MEMORY);
    TextureManager::Get()->Load("DefaultMaterial_Normal", "data/textures/DefaultTextures/DefaultMaterial_Normal.png", STATIC_MEMORY);
    TextureManager::Get()->Load("DefaultMaterial_Specular", "data/textures/DefaultTextures/DefaultMaterial_Specular.png", STATIC_MEMORY);
    TextureManager::Get()->Load("DefaultMaterial_Noise", "data/textures/DefaultTextures/DefaultMaterial_Noise.png", STATIC_MEMORY);
    TextureManager::Get()->Load("portal", "data/textures/PSX Textures/256/Color/liquid_alien_1.png", STATIC_MEMORY);
    TextureManager::Get()->Load("magma", "data/textures/PSX Textures/256/Color/stone_hell_10.png", STATIC_MEMORY);
    TextureManager::Get()->Load("fence", "data/textures/Chain Fence.png", STATIC_MEMORY);


    // Load Materials
    MaterialManager::Get()->LoadTexture("DefaultMaterial",
        FragmentShaderManager::Get()->Get("default"),
        TextureManager::Get()->Get("DefaultMaterial_Diffuse"),
        TextureManager::Get()->Get("DefaultMaterial_Normal"),
        TextureManager::Get()->Get("DefaultMaterial_Specular"),
        64, STATIC_MEMORY);

    MaterialManager::Get()->LoadPortal("AlienPortalMaterial",
        FragmentShaderManager::Get()->Get("portal_frag"),
        TextureManager::Get()->Get("portal"),
        TextureManager::Get()->Get("DefaultMaterial_Noise"), STATIC_MEMORY);

    MaterialManager::Get()->LoadPortal("Level1MagmaMaterial",
        FragmentShaderManager::Get()->Get("magma_frag"),
        TextureManager::Get()->Get("magma"),
        TextureManager::Get()->Get("DefaultMaterial_Noise"), STATIC_MEMORY);
    
    MaterialManager::Get()->LoadPortal("MetalFenceMaterial",
        FragmentShaderManager::Get()->Get("fence_frag"),
        TextureManager::Get()->Get("fence"),
        TextureManager::Get()->Get("DefaultMaterial_Noise"), STATIC_MEMORY);
        
    // Load Models
    ModelManager::Get()->Load("box", "data/models/cube.fbx", STATIC_MEMORY);
}