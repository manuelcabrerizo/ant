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
     inputSystem.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     // Register system to be notify
     NotificationManager::Get()->RegisterListener(&weaponSystem, NOTIFICATION_SHOOT);

     // Create sniper
     firstWeapon = CreateActorFromFile("../data/xml/sniper.xml", 
          &actorManager, &textureManager, &modelManager);

     // Create pistol
     secondWeapon = CreateActorFromFile("../data/xml/pistol.xml",
          &actorManager, &textureManager, &modelManager);
     actorManager.RemoveRenderComponent(secondWeapon);

     // Create Player
     player = CreateActorFromFile("../data/xml/player.xml",
          &actorManager, &textureManager, &modelManager);
     actorManager.AddWeaponComponent(player, firstWeapon);

     // Create Level
     SlotmapKey<Actor> level = actorManager.CreateActor();
     actorManager.AddTransformComponent(level, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     actorManager.AddRenderComponent(level, walls->model, textureManager.Get("default")->texture);
     
     collisionWorld.LoadFromFile("../data/collision/level-collision.obj");
}

void Game::Update(f32 dt)
{     
     GraphicsManager::Get()->BeginFrame(1.0f, 0.0f, 1.0f);
     
     inputSystem.Update(&actorManager, &collisionWorld, dt);
     cameraSystem.Update(&actorManager, dt);
     weaponSystem.Update(&actorManager, dt);

     ModelHandle *pistol = modelManager.Get("pistol");
     ModelHandle *sniper = modelManager.Get("sniper");

     Texture *texture = textureManager.Get("default")->texture;
     
     if(InputManager::Get()->KeyJustDown(KEY_1) && !usingFirstWeapon)
     {
          actorManager.RemoveRenderComponent(secondWeapon);
          actorManager.AddRenderComponent(firstWeapon, sniper->model, texture);
          
          actorManager.RemoveWeaponComponent(player);
          actorManager.AddWeaponComponent(player, firstWeapon);

          usingFirstWeapon = true;

          actorManager.PrintActorAndCompoenentState();
     }
     if(InputManager::Get()->KeyJustDown(KEY_2) && usingFirstWeapon)
     {
          actorManager.RemoveRenderComponent(firstWeapon);
          actorManager.AddRenderComponent(secondWeapon, pistol->model, texture);
          
          actorManager.RemoveWeaponComponent(player);
          actorManager.AddWeaponComponent(player, secondWeapon);

          usingFirstWeapon = false;

          actorManager.PrintActorAndCompoenentState();
     }
}

void Game::Render()
{    
     GraphicsManager::Get()->ShaderBind(shader);

     renderSystem.Render(&actorManager, 0.0f);
     
     GraphicsManager::Get()->EndFrame(1);
}

void Game::Terminate()
{
     inputSystem.Terminate();
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
