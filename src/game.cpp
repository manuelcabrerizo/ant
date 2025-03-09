void Game::Init()
{
     printf("Game Init!\n");

     // Allocate memory for the entire game
     gameArena.Init(MB(10));

     // Load the shader
     File vertFile = PlatformReadFile("../data/shaders/vert.hlsl", &gameArena);
     File fragFile = PlatformReadFile("../data/shaders/frag.hlsl", &gameArena);
     shader = GraphicsManager::ShaderAlloc(vertFile, fragFile);
     
     // Load the models
     cube.Init("../data/models/cube.obj", &gameArena);
     plane.Init("../data/models/plane.obj", &gameArena);
     pistol.Init("../data/models/pistol.obj", &gameArena);
     sniper.Init("../data/models/sniper.obj", &gameArena);
     warrior.Init("../data/models/warrior.dae", &gameArena);
     wall.Init("../data/models/wall.obj", &gameArena);


     // Load a texture
     texture = GraphicsManager::TextureAlloc("../data/textures/texture_13.png");
     warriorTexture = GraphicsManager::TextureAlloc("../data/textures/warrior.png");

     // Reset the memory for runtime use
     gameArena.Clear();

     // Initialize the Actor Manager
     am.Init(&gameArena);

     // Initialize gameplay systems
     inputSystem.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     // Create sniper
     firstWeapon = am.CreateActor();
     am.AddTransformComponent(firstWeapon, vec3(2.0f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(firstWeapon, sniper, texture);

     // Create pistol
     secondWeapon = am.CreateActor();
     am.AddTransformComponent(secondWeapon, vec3(1.5f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));

     // Create Player
     player = am.CreateActor();
     am.AddTransformComponent(player, vec3(0.0f, 0.25f, -4.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddCameraComponent(player, vec3(0.0f, 0.0f, -4.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddInputComponent(player);
     am.AddWeaponComponent(player, firstWeapon);

     // Create wallRH
     SlotmapKey<Actor> rh = am.CreateActor();
     am.AddTransformComponent(rh, vec3(1.0f, 0.5f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(rh, wall, texture);
     
     // Create box
     SlotmapKey<Actor> box = am.CreateActor();
     am.AddTransformComponent(box, vec3(1.0f, 0.0f, -4.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(box, cube, texture);
     
     SlotmapKey<Actor> guy = am.CreateActor();
     am.AddTransformComponent(guy, vec3(-1.0f, -0.5f, -1.0f), vec3(0.005f, 0.005f, 0.005f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(guy, warrior, warriorTexture);

     // Create floor
     SlotmapKey<Actor> floor = am.CreateActor();
     am.AddTransformComponent(floor, vec3(0.0f), vec3(1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(floor, plane, texture);

     printf("Allocation count: %d\n", Arena::allocationCount);
}

void Game::Update(InputManager *input, f32 dt)
{
     inputSystem.Update(input, &am, dt);
     cameraSystem.Update(&am, dt);
     weaponSystem.Update(&am, dt);
     
     if(input->KeyJustDown(KEY_1) && !usingFirstWeapon)
     {
          am.RemoveRenderComponent(secondWeapon);
          am.AddRenderComponent(firstWeapon, sniper, texture);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, firstWeapon);

          usingFirstWeapon = true;
     }
     if(input->KeyJustDown(KEY_2) && usingFirstWeapon)
     {
          am.RemoveRenderComponent(firstWeapon);
          am.AddRenderComponent(secondWeapon, pistol, texture);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, secondWeapon);

          usingFirstWeapon = false;
     }
}

void Game::Render()
{    
     GraphicsManager::BeginFrame(1.0f, 0.0f, 1.0f);

     GraphicsManager::ShaderBind(shader);

     renderSystem.Render(&am, 0.0f);
     
     GraphicsManager::EndFrame(1);
}

void Game::Terminate()
{
     inputSystem.Terminate();
     cameraSystem.Terminate();
     renderSystem.Terminate();
     weaponSystem.Terminate();
     
     am.Terminate();

     GraphicsManager::TextureFree(warriorTexture);
     GraphicsManager::TextureFree(texture);
     GraphicsManager::ShaderFree(shader);
     cube.Terminate();
     plane.Terminate();
     pistol.Terminate();
     sniper.Terminate();
     warrior.Terminate();
     wall.Terminate();

     gameArena.Terminate();
     
     printf("Game Terminate!\n");
}
