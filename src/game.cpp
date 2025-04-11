// TODO:
// - More colliders and Raycast test
// - Implement a small demo with a full loop
// the player has to have life and be able to shoot at least two weapons
// the enemies have to move arrown the map and shoot the player
// when the player kills all enemies the game restart
// when the player gets kill the game restart

void Game::Init()
{
     printf("Game Init!\n");

     //GraphicsManager::Get()->DebugInit();
     
     // Load the shader
     Frame frame = MemoryManager::Get()->GetFrame();
     File vertFile = PlatformReadFile("../data/shaders/vert.hlsl", FRAME_MEMORY);
     File fragFile = PlatformReadFile("../data/shaders/frag.hlsl", FRAME_MEMORY);
     shader = GraphicsManager::Get()->ShaderAlloc(vertFile, fragFile);
     MemoryManager::Get()->ReleaseFrame(frame);
     
     // Load the models
     modelManager.Init(32);
     modelManager.Load("cube", "../data/models/cube.obj");
     modelManager.Load("plane", "../data/models/plane.obj");
     modelManager.Load("pistol", "../data/models/pistol.obj");
     modelManager.Load("sniper", "../data/models/sniper.obj");
     modelManager.Load("test-level", "../data/models/level-rendering.obj");

     ModelHandle *cube = modelManager.Get("cube");
     ModelHandle *plane = modelManager.Get("plane");
     ModelHandle *pistol = modelManager.Get("pistol");
     ModelHandle *sniper = modelManager.Get("sniper");
     ModelHandle *walls = modelManager.Get("test-level");

     // Load a texture
     textureManager.Init(128);
     textureManager.Load("default", "../data/textures/GridBox_Default.png");
     
     // Initialize the Actor Manager
     actorManager.Init(STATIC_MEMORY);

     // Initialize gameplay systems
     playerController.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     // Register system to be notify
     NotificationManager::Get()->RegisterListener(&weaponSystem, NOTIFICATION_SHOOT);

     // Create Player
     SlotmapKey<Actor> player = CreateActorFromFile("../data/xml/player.xml",
          &actorManager, &textureManager, &modelManager);
     actorManager.AddPhysicsComponent(player, vec3(0.0), vec3(0.0f));

     // Create Level
     SlotmapKey<Actor> level = actorManager.CreateActor();
     actorManager.AddTransformComponent(level, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     actorManager.AddRenderComponent(level, walls->model, textureManager.Get("default")->texture);
     
     // Create collision world
     collisionWorld.LoadFromFile("../data/collision/level-collision.obj");
}

void Game::Update(f32 dt)
{    
     // NOTE: this is in the update for the debug renderer to work properly 
     GraphicsManager::Get()->BeginFrame(1.0f, 0.0f, 1.0f);
     
     playerController.Update(&actorManager, &collisionWorld, dt);
     cameraSystem.Update(&actorManager, dt);
     weaponSystem.Update(&actorManager, dt);
}

void Game::Render()
{    
     GraphicsManager::Get()->ShaderBind(shader);

     renderSystem.Render(&actorManager, 0.0f);
     
     GraphicsManager::Get()->EndFrame(1);
}

void Game::Terminate()
{
     playerController.Terminate();
     cameraSystem.Terminate();
     renderSystem.Terminate();
     weaponSystem.Terminate();
     
     actorManager.Terminate();
     textureManager.Terminate();
     modelManager.Terminate();

     GraphicsManager::Get()->ShaderFree(shader);

     //GraphicsManager::Get()->DebugTerminate();
     
     printf("Game Terminate!\n");
}
