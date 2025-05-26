// TODO:
// - More colliders and Raycast test
// - Basic steering behaviors
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
     shaderManager.Init(4);
     shaderManager.Load("default", "data/shaders/vert.hlsl", "data/shaders/frag.hlsl");
     shaderManager.Load("animation", "data/shaders/animation_vert.hlsl", "data/shaders/frag.hlsl");
     shaderManager.Bind("default");
     
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

     // Load a texture
     textureManager.Init(128);
     textureManager.Load("default", "data/textures/GridBox_Default.png");
     textureManager.Load("warrior", "data/textures/warrior.png");
     
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
     SlotmapKey<Actor> player = actorManager.CreateActorFromFile("data/xml/player.xml", &textureManager, &modelManager);
     WeaponComponent *weapon = actorManager.GetComponent<WeaponComponent>(player);
     AnimationComponent animationCmp;
     animationCmp.skeleton.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", STATIC_MEMORY);
     animationCmp.animation.Init("data/models/fps-animations-vsk/source/FPS_VSK1.fbx", modelManager.Get("anim-gun"), STATIC_MEMORY);
     actorManager.AddComponent<AnimationComponent>(weapon->weapon, animationCmp);
     RenderComponent *render = actorManager.GetComponent<RenderComponent>(weapon->weapon);
     render->isAnimated = true;
     

     actorManager.CreateActorFromFile("data/xml/test-level.xml", &textureManager, &modelManager);
     actorManager.CreateActorFromFile("data/xml/house.xml", &textureManager, &modelManager);
     SlotmapKey<Actor> enemy[3] =
     {
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &textureManager, &modelManager),
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &textureManager, &modelManager),
          actorManager.CreateActorFromFile("data/xml/enemy.xml", &textureManager, &modelManager)
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
     actorManager.RenderComponents<RenderComponent>(&shaderManager);
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
     
     actorManager.Terminate();
     textureManager.Terminate();
     modelManager.Terminate();
     shaderManager.Terminate();

     printf("Game Terminate!\n");
}
