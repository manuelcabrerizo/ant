// TODO:
// - More colliders and Raycast test
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

     // Initialize gameplay systems
     playerController.Init();
     physicsSystem.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     // Register system to be notify
     NotificationManager::Get()->RegisterListener(&weaponSystem, NOTIFICATION_SHOOT);

     // Create Entities
     CreateActorFromFile("../data/xml/player.xml", &actorManager, &textureManager, &modelManager);
     CreateActorFromFile("../data/xml/enemy.xml", &actorManager, &textureManager, &modelManager);
     CreateActorFromFile("../data/xml/test-level.xml", &actorManager, &textureManager, &modelManager);
     
     // Create collision world
     collisionWorld.LoadFromFile("../data/collision/level-collision.obj");
     
     // TODO: GraphicsManager::Get()->DebugInit();
     printf("Game Init!\n");
}

void Game::Update(f32 dt)
{    
     playerController.Update(&actorManager, dt);
     cameraSystem.Update(&actorManager, dt);
     weaponSystem.Update(&actorManager, dt);
     physicsSystem.Update(&actorManager, &collisionWorld, dt);
}

void Game::Render()
{    
     GraphicsManager::Get()->BeginFrame(1.0f, 0.0f, 1.0f);
     renderSystem.Render(&actorManager, 0.0f);
     GraphicsManager::Get()->EndFrame(1);
}

void Game::Terminate()
{
     // TODO: GraphicsManager::Get()->DebugTerminate();

     playerController.Terminate();
     physicsSystem.Terminate();
     cameraSystem.Terminate();
     renderSystem.Terminate();
     weaponSystem.Terminate();
     
     actorManager.Terminate();
     textureManager.Terminate();
     modelManager.Terminate();
     shaderManager.Terminate();

     printf("Game Terminate!\n");
}
