void Game::Init()
{
     printf("Game Init!\n");

     gameArena.Init(KB(600));

     // Load the shader
     File vertFile = PlatformReadFile("../data/shaders/vert.hlsl", &gameArena);
     File fragFile = PlatformReadFile("../data/shaders/frag.hlsl", &gameArena);
     shader = GraphicsManager::ShaderAlloc(vertFile, fragFile);
     // Load the models
     cube.Init("../data/models/cube.obj", &gameArena);
     plane.Init("../data/models/plane.obj", &gameArena);
     pistol.Init("../data/models/pistol.obj", &gameArena);
     sniper.Init("../data/models/sniper.obj", &gameArena);
     // Load a texture
     texture = GraphicsManager::TextureAlloc("../data/textures/texture_13.png");

     gameArena.Clear();

     am.Init(&gameArena);
     
     inputSystem.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     firstWeapon = am.CreateActor();
     am.AddTransformComponent(firstWeapon, vec3(2.0f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(firstWeapon, sniper);

     secondWeapon = am.CreateActor();
     am.AddTransformComponent(secondWeapon, vec3(1.5f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));
     
     vec3 dir = vec3(0.0f, 0.0f, 1.0f);
     player = am.CreateActor();
     am.AddTransformComponent(player, vec3(0.0f, 0.0f, -4.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddCameraComponent(player, vec3(0.0f, 0.0f, -4.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddInputComponent(player);
     am.AddWeaponComponent(player, firstWeapon);

     SlotmapKey<Actor> box = am.CreateActor();
     am.AddTransformComponent(box, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(box, cube);

     SlotmapKey<Actor> floor = am.CreateActor();
     am.AddTransformComponent(floor, vec3(0.0f), vec3(1.0f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(floor, plane);

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
          am.AddRenderComponent(firstWeapon, sniper);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, firstWeapon);

          usingFirstWeapon = true;
     }
     if(input->KeyJustDown(KEY_2) && usingFirstWeapon)
     {
          am.RemoveRenderComponent(firstWeapon);
          am.AddRenderComponent(secondWeapon, pistol);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, secondWeapon);

          usingFirstWeapon = false;
     }
}

void Game::Render()
{    
     GraphicsManager::BeginFrame(0.125f, 0.05f, 0.05f);

     GraphicsManager::ShaderBind(shader);
     GraphicsManager::TextureBind(texture, 0);

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
     
     GraphicsManager::TextureFree(texture);
     GraphicsManager::ShaderFree(shader);
     cube.Terminate();
     plane.Terminate();
     pistol.Terminate();
     sniper.Terminate();

     gameArena.Terminate();
     
     printf("Game Terminate!\n");
}
