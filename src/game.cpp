// TODO:
// - Implement a small demo with a full loop
// the player has to have life and be able to shoot at least two weapons
// the enemies have to move arrown the map and shoot the player
// when the player kills all enemies the game restart
// when the player gets kill the game restart

#include <game.h>

#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <asset_managers/material_manager.h>
#include <asset_managers/model_manager.h>

#include <components/component.h>
#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/physics_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/anchor_component.h>
#include <components/player_controller_component.h>
#include <components/enemy_component.h>
#include <components/animation_component.h>

void Game::Init()
{
    // Move this initialization to the engine
    VertexShaderManager::Initialize(4);
    FragmentShaderManager::Initialize(4);
    TextureManager::Initialize(64);
    MaterialManager::Initialize(64);
    ModelManager::Initialize(32);

     // Load Vertex the shaders
     VertexShaderManager::Get()->Load("default", "data/shaders/vert.hlsl");
     VertexShaderManager::Get()->Load("animation", "data/shaders/animation_vert.hlsl");
     VertexShaderManager::Get()->Bind("default");

     // Load Fragment the shaders
     FragmentShaderManager::Get()->Load("default", "data/shaders/frag.hlsl");
     FragmentShaderManager::Get()->Load("color", "data/shaders/color.hlsl");
     FragmentShaderManager::Get()->Bind("default");
     
     // Load textures
     TextureManager::Get()->Load("DefaultMaterial_Diffuse", "data/textures/DefaultTextures/DefaultMaterial_Diffuse.png");
     TextureManager::Get()->Load("DefaultMaterial_Normal", "data/textures/DefaultTextures/DefaultMaterial_Normal.png");
     TextureManager::Get()->Load("DefaultMaterial_Specular", "data/textures/DefaultTextures/DefaultMaterial_Specular.png");

     // Load Materials
     MaterialManager::Get()->LoadTexture("DefaultMaterial", "default",
         "DefaultMaterial_Diffuse", "DefaultMaterial_Normal", "DefaultMaterial_Specular", 64);
     MaterialManager::Get()->LoadSolidColor("GreenMaterial", "color",
         Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.6f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);
     MaterialManager::Get()->LoadSolidColor("RedMaterial", "color",
         Vector3(1.0f, 0.0f, 0.0f), Vector3(0.6f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);
     MaterialManager::Get()->LoadSolidColor("YellowMaterial", "color",
         Vector3(1.0f, 1.0f, 0.0f), Vector3(0.6f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);

     // Load the models
     ModelManager::Get()->Load("house", "data/models/House/source/house.fbx");
     ModelManager::Get()->Load("warrior", "data/models/Warrior/source/warrior.fbx");
     ModelManager::Get()->Load("test-level", "data/models/Level/source/level.fbx");
     ModelManager::Get()->Load("anim-gun", "data/models/fps-animations-vsk/source/FPS_VSK1.fbx");
     ModelManager::Get()->Load("tower", "data/models/MagicStudio/source/MagicStudio.fbx");
     ModelManager::Get()->Load("wizard", "data/models/Wizard/source/Wizard.FBX");

     // Initialize the Actor Manager
     actorManager.Init(100, 64, STATIC_MEMORY);
     actorManager.AddComponentType<TransformComponent, 100>();
     actorManager.AddComponentType<RenderComponent, 100>();
     actorManager.AddComponentType<PhysicsComponent, 100>();
     actorManager.AddComponentType<PlayerControllerComponent, 1>();
     actorManager.AddComponentType<WeaponComponent, 100>();
     actorManager.AddComponentType<CameraComponent, 1>();
     actorManager.AddComponentType<EnemyComponent, 10>();
     actorManager.AddComponentType<AnchorComponent, 10>();
     actorManager.AddComponentType<AnimationComponent, 10>();
     actorManager.AllocInternalMemory();

     // Create Entities
     player = actorManager.CreateActorFromFile("data/xml/player.xml");
     //WeaponComponent *weapon = actorManager.GetComponent<WeaponComponent>(player);
     //AnimationComponent animationCmp;
     //animationCmp.skeleton.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", STATIC_MEMORY);
     //animationCmp.animation.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", ModelManager::Get()->Get("anim-gun"), STATIC_MEMORY);
     //actorManager.AddComponent<AnimationComponent>(weapon->weapon, animationCmp);
     //RenderComponent *render = actorManager.GetComponent<RenderComponent>(weapon->weapon);
     //render->isAnimated = true;
     
     actorManager.CreateActorFromFile("data/xml/test-level.xml");
     actorManager.CreateActorFromFile("data/xml/tower.xml");
     actorManager.CreateActorFromFile("data/xml/house.xml");
     actorManager.CreateActorFromFile("data/xml/wizard.xml");
     
     /*
     SlotmapKey<Actor> enemy[3] =
     {
          actorManager.CreateActorFromFile("data/xml/enemy.xml"),
          actorManager.CreateActorFromFile("data/xml/enemy.xml"),
          actorManager.CreateActorFromFile("data/xml/enemy.xml")
     };
     

     // Warrior Animation
     TransformComponent *transforms[3] =
     {
          actorManager.GetComponent<TransformComponent>(enemy[0]),
          actorManager.GetComponent<TransformComponent>(enemy[1]),
          actorManager.GetComponent<TransformComponent>(enemy[2])
     };
     transforms[0]->position.x = 0.0f;
     transforms[1]->position.x = -2.0f;
     transforms[2]->position.x = -4.0f;

     RenderComponent *renders[3] =
     {
          actorManager.GetComponent<RenderComponent>(enemy[0]),
          actorManager.GetComponent<RenderComponent>(enemy[1]),
          actorManager.GetComponent<RenderComponent>(enemy[2])
     };
     renders[0]->isAnimated = true;
     renders[1]->isAnimated = true;
     renders[2]->isAnimated = true;

     AnimationComponent animation;
     animation.skeleton.Init("data/models/warrior.dae", STATIC_MEMORY);
     animation.animation.Init("data/animations/walk_front.dae", ModelManager::Get()->Get("warrior"), STATIC_MEMORY);
     actorManager.AddComponent<AnimationComponent>(enemy[0], animation);
     actorManager.AddComponent<AnimationComponent>(enemy[1], animation);
     actorManager.AddComponent<AnimationComponent>(enemy[2], animation);
     */

     CameraComponent::Initialize();
     RenderComponent::Initialize();
     PhysicsComponent::Initialize();

     GraphicsManager::Get()->DebugInit();
     printf("Game Init!\n"); 
}

void Game::Update(f32 dt)
{
    // TODO: this is down here for the debug renderer
    GraphicsManager::Get()->BeginFrame(0.2f, 0.2f, 0.4f);

     // Initialize new components
     actorManager.InitializeNewComponents();
     // Update
     actorManager.UpdateComponents<PlayerControllerComponent>(dt);
     actorManager.UpdateComponents<CameraComponent>(dt);
     actorManager.UpdateComponents<WeaponComponent>(dt);
     actorManager.UpdateComponents<EnemyComponent>(dt);
     actorManager.UpdateComponents<PhysicsComponent>(dt);
     actorManager.UpdateComponents<RenderComponent>(dt);
     // Late Update
     actorManager.LateUpdateComponents<PlayerControllerComponent>(dt);
}

#include <math/matrix4.h>

void Game::Render(f32 dt)
{
    actorManager.RenderComponents<RenderComponent>();

    GraphicsManager::Get()->DebugPresent();
    GraphicsManager::Get()->EndFrame(1);
}

void Game::Terminate()
{
     GraphicsManager::Get()->DebugTerminate();

     CameraComponent::Terminate();
     RenderComponent::Terminate();
     PhysicsComponent::Terminate();
     
     actorManager.Terminate();
     
     MaterialManager::Shutdown();
     TextureManager::Shutdown();
     ModelManager::Shutdown();
     VertexShaderManager::Shutdown();
     FragmentShaderManager::Shutdown();

     printf("Game Terminate!\n");
}
