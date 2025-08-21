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

void GameManager::Init()
{
    InitializeAssetsManagers();

    CollisionWorld::Init(100);

    GraphicsManager::Get()->DebugInit();

    LoadDefaultAssets();

    // Load the models
    ModelManager::Get()->Load("house", "data/models/House/source/house.fbx");
    ModelManager::Get()->Load("warrior", "data/models/Warrior/source/warrior.fbx");
    ModelManager::Get()->Load("test-level", "data/models/Level/source/level.fbx");
    ModelManager::Get()->Load("anim-gun", "data/models/fps-animations-vsk/source/FPS_VSK1.fbx");
    ModelManager::Get()->Load("tower", "data/models/MagicStudio/source/MagicStudio.fbx");
    ModelManager::Get()->Load("wizard", "data/models/Wizard/source/Wizard.FBX");
    ModelManager::Get()->Load("bullet", "data/models/testBullet.fbx");
    ModelManager::Get()->Load("level1", "data/models/Level1/source/Level.fbx");

    scenes.Init(1, STATIC_MEMORY);

    menuState.Init(this);
    playState.Init(this);
    stateMachine.Push(&menuState);
}

void GameManager::Update(f32 dt)
{
    GraphicsManager::Get()->BeginFrame(0.2f, 0.2f, 0.4f);
    CollisionWorld::Get()->DebugDraw();
    stateMachine.Update(dt);

    size_t freeMemory = MemoryManager::Get()->GetFreeMemoryCount(FRAME_MEMORY);
    char buffer[256];
    sprintf(buffer, "free memory: %zu\n", freeMemory);
    OutputDebugString(buffer);
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
    menuState.Terminate();

    GraphicsManager::Get()->DebugTerminate();

    CollisionWorld::Terminate();

    // Load the models
    ModelManager::Get()->Unload("house");
    ModelManager::Get()->Unload("warrior");
    ModelManager::Get()->Unload("test-level");
    ModelManager::Get()->Unload("anim-gun");
    ModelManager::Get()->Unload("tower");
    ModelManager::Get()->Unload("wizard");
    ModelManager::Get()->Unload("bullet");
    ModelManager::Get()->Unload("level1");

    ShutdownAssetsManagers();
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