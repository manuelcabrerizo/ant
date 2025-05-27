// TODO:
// - add support to multiple texture per model
// - add support for materials instead of just texutres
// - More colliders and Raycast test
// - Implement a small demo with a full loop
// the player has to have life and be able to shoot at least two weapons
// the enemies have to move arrown the map and shoot the player
// when the player kills all enemies the game restart
// when the player gets kill the game restart

#include <game.h>

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

#include <stdio.h>

void Game::Init()
{
     // Load the shaders
     vertexShaderManager.Init(4);
     vertexShaderManager.Load("default", "data/shaders/vert.hlsl");
     vertexShaderManager.Load("animation", "data/shaders/animation_vert.hlsl");
     vertexShaderManager.Bind("default");

     fragmentShaderManager.Init(4);
     fragmentShaderManager.Load("default", "data/shaders/frag.hlsl");
     fragmentShaderManager.Load("color", "data/shaders/color.hlsl");
     fragmentShaderManager.Bind("default");
     
     // Load the models
     modelManager.Init(32);
     modelManager.Load("cube", "data/models/cube.obj");
     modelManager.Load("sphere", "data/models/sphere.obj");
     modelManager.Load("plane", "data/models/plane.obj");
     modelManager.Load("pistol", "data/models/pistol.obj");
     modelManager.Load("sniper", "data/models/sniper.obj");
     modelManager.Load("warrior", "data/models/warrior.dae");
     modelManager.Load("test-level", "data/models/level-rendering.obj");
     modelManager.Load("anim-gun", "data/models/fps-animations-vsk/source/FPS_VSK1.fbx");
     modelManager.Load("house", "data/models/house.fbx");
     modelManager.Load("tower", "data/models/MagicTower/MagicStudio.fbx");

     // Load a texture
     textureManager.Init(128);
     textureManager.Load("DefaultMaterial_Diffuse", "data/textures/DefaultTextures/DefaultMaterial_Diffuse.png");
     textureManager.Load("DefaultMaterial_Normal", "data/textures/DefaultTextures/DefaultMaterial_Normal.png");
     textureManager.Load("DefaultMaterial_Specular", "data/textures/DefaultTextures/DefaultMaterial_Specular.png");
     //textureManager.Load("default", "data/textures/GridBox_Default.png");
     //textureManager.Load("warrior", "data/textures/warrior.png");

     // Material Manager Test
     materialManager.Init(32);

     materialManager.LoadTexture("DefaultMaterial",
         "default", &fragmentShaderManager,
         "DefaultMaterial_Diffuse",
         "DefaultMaterial_Normal",
         "DefaultMaterial_Specular",
         64, &textureManager);

     materialManager.LoadSolidColor("GreenMaterial", "color", &fragmentShaderManager,
           Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.6f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);
     materialManager.LoadSolidColor("RedMaterial", "color", &fragmentShaderManager,
         Vector3(1.0f, 0.0f, 0.0f), Vector3(0.6f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);
     materialManager.LoadSolidColor("YellowMaterial", "color", &fragmentShaderManager,
         Vector3(1.0f, 1.0f, 0.0f), Vector3(0.6f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 32);
     

     
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
     SlotmapKey<Actor> player = actorManager.CreateActorFromFile("data/xml/player.xml", &modelManager, &materialManager);
     WeaponComponent *weapon = actorManager.GetComponent<WeaponComponent>(player);
     AnimationComponent animationCmp;
     animationCmp.skeleton.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", STATIC_MEMORY);
     animationCmp.animation.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", modelManager.Get("anim-gun"), STATIC_MEMORY);
     actorManager.AddComponent<AnimationComponent>(weapon->weapon, animationCmp);
     RenderComponent *render = actorManager.GetComponent<RenderComponent>(weapon->weapon);
     render->isAnimated = true;
     

     actorManager.CreateActorFromFile("data/xml/test-level.xml", &modelManager, &materialManager);
     actorManager.CreateActorFromFile("data/xml/house.xml", &modelManager, &materialManager);
     actorManager.CreateActorFromFile("data/xml/tower.xml", &modelManager, &materialManager);
     SlotmapKey<Actor> enemy[3] =
     {
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &modelManager, &materialManager),
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &modelManager, &materialManager),
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &modelManager, &materialManager)
     };
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
     animation.animation.Init("data/animations/walk_front.dae", modelManager.Get("warrior"), STATIC_MEMORY);
     actorManager.AddComponent<AnimationComponent>(enemy[0], animation);
     actorManager.AddComponent<AnimationComponent>(enemy[1], animation);
     actorManager.AddComponent<AnimationComponent>(enemy[2], animation);

     
     CameraComponent::Initialize();
     RenderComponent::Initialize();
     PhysicsComponent::Initialize();

     GraphicsManager::Get()->DebugInit();
     printf("Game Init!\n"); 
}

void Game::Update(f32 dt)
{    
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

void Game::Render()
{
     actorManager.RenderComponents<RenderComponent>(&vertexShaderManager);

     GraphicsManager::Get()->DebugPresent();
     GraphicsManager::Get()->EndFrame(1);

     GraphicsManager::Get()->BeginFrame(0.2f, 0.2f, 0.4f);
}

void Game::Terminate()
{
     GraphicsManager::Get()->DebugTerminate();

     CameraComponent::Terminate();
     RenderComponent::Terminate();
     PhysicsComponent::Terminate();
     
     materialManager.Terminate();

     actorManager.Terminate();
     textureManager.Terminate();
     modelManager.Terminate();
     vertexShaderManager.Terminate();
     fragmentShaderManager.Terminate();

     printf("Game Terminate!\n");
}
