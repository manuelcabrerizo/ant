// TODO:
// - Try to implement the gameplay login on the component itself
// - Get the components array on the init of the systems
// - More colliders and Raycast test
// - Basic steering behaviors
// - Implement a small demo with a full loop
// the player has to have life and be able to shoot at least two weapons
// the enemies have to move arrown the map and shoot the player
// when the player kills all enemies the game restart
// when the player gets kill the game restart

void Game::Init()
{
     // Load the shaders
     shaderManager.Init(4);
     shaderManager.Load("default", "../data/shaders/vert.hlsl", "../data/shaders/frag.hlsl");
     shaderManager.Bind("default");
     
     // Load the models
     modelManager.Init(32);
     modelManager.Load("cube", "../data/models/cube.obj");
     modelManager.Load("sphere", "../data/models/sphere.obj");
     modelManager.Load("plane", "../data/models/plane.obj");
     modelManager.Load("pistol", "../data/models/pistol.obj");
     modelManager.Load("sniper", "../data/models/sniper.obj");
     modelManager.Load("warrior", "../data/models/warrior.dae");
     modelManager.Load("test-level", "../data/models/level-rendering.obj");

     // Load a texture
     textureManager.Init(128);
     textureManager.Load("default", "../data/textures/GridBox_Default.png");
     textureManager.Load("warrior", "../data/textures/warrior.png");
     
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

     // Create Entities
     actorManager.CreateActorFromFile("../data/xml/player.xml", &textureManager, &modelManager);
     actorManager.CreateActorFromFile("../data/xml/test-level.xml", &textureManager, &modelManager);
     SlotmapKey<Actor> enemy[3] =
     {
          actorManager.CreateActorFromFile("../data/xml/enemy.xml", &textureManager, &modelManager),
          actorManager.CreateActorFromFile("../data/xml/enemy.xml", &textureManager, &modelManager),
          actorManager.CreateActorFromFile("../data/xml/enemy.xml", &textureManager, &modelManager)
     };
     TransformComponent *transforms[3] =
     {
          actorManager.GetComponent<TransformComponent>(enemy[0]),
          actorManager.GetComponent<TransformComponent>(enemy[1]),
          actorManager.GetComponent<TransformComponent>(enemy[2])
     };
     transforms[0]->position.x = 0.0f;
     transforms[1]->position.x = 3.0f;
     transforms[2]->position.x = 6.0f;

     CameraComponent::Initialize();
     RenderComponent::Initialize();
     PhysicsComponent::Initialize();

     actorManager.InitComponents<CameraComponent>();
     actorManager.InitComponents<RenderComponent>();
     actorManager.InitComponents<PhysicsComponent>();

     // Initialize gameplay systems
     playerController.Init();
     weaponSystem.Init();
     enemySystem.Init();

     // Register system to be notify
     NotificationManager::Get()->RegisterListener(&weaponSystem, NOTIFICATION_SHOOT);
     NotificationManager::Get()->RegisterListener(&enemySystem, NOTIFICATION_PLAYER_MOVE);

     GraphicsManager::Get()->DebugInit();
     printf("Game Init!\n");
}

void Game::Update(f32 dt)
{    
     playerController.Update(&actorManager, dt);
     actorManager.UpdateComponents<CameraComponent>(dt);
     weaponSystem.Update(&actorManager, dt);
     enemySystem.Update(&actorManager, dt);
     actorManager.UpdateComponents<PhysicsComponent>(dt);

     playerController.LateUpdate(&actorManager, dt);
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

     actorManager.TerminateComponents<CameraComponent>();
     actorManager.TerminateComponents<RenderComponent>();
     actorManager.TerminateComponents<PhysicsComponent>();


     playerController.Terminate();
     enemySystem.Terminate();

     weaponSystem.Terminate();

     CameraComponent::Terminate();
     RenderComponent::Terminate();
     PhysicsComponent::Terminate();
     
     actorManager.Terminate();
     textureManager.Terminate();
     modelManager.Terminate();
     shaderManager.Terminate();

     printf("Game Terminate!\n");
}
