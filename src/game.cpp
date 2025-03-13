SlotmapKey<Actor> CreateActorFromFile(const char *filepath, ActorManager *am)
{
     tinyxml2::XMLDocument doc;
     doc.LoadFile(filepath);

     SlotmapKey<Actor> actor = am->CreateActor();

     tinyxml2::XMLElement *root = doc.FirstChildElement("Actor");

     tinyxml2::XMLElement *component = root->FirstChildElement();
     while(component)
     {
          const char *componentType = component->Value();

          if(strcmp("TransformComponent", componentType) == 0)
          {
               tinyxml2::XMLElement *attributes = component->FirstChildElement();
               vec3 position;
               attributes->QueryFloatAttribute("x", &position.x);
               attributes->QueryFloatAttribute("y", &position.y);
               attributes->QueryFloatAttribute("z", &position.z);
               attributes = attributes->NextSiblingElement();
               vec3 scale;
               attributes->QueryFloatAttribute("x", &scale.x);
               attributes->QueryFloatAttribute("y", &scale.y);
               attributes->QueryFloatAttribute("z", &scale.z);
               attributes = attributes->NextSiblingElement();
               vec3 direction;
               attributes->QueryFloatAttribute("x", &direction.x);
               attributes->QueryFloatAttribute("y", &direction.y);
               attributes->QueryFloatAttribute("z", &direction.z);
               am->AddTransformComponent(actor, position, scale, direction);
          }
          else if(strcmp("CameraComponent", componentType) == 0)
          {
               tinyxml2::XMLElement *attributes = component->FirstChildElement();
               vec3 position;
               attributes->QueryFloatAttribute("x", &position.x);
               attributes->QueryFloatAttribute("y", &position.y);
               attributes->QueryFloatAttribute("z", &position.z);
               attributes = attributes->NextSiblingElement();
               vec3 direction;
               attributes->QueryFloatAttribute("x", &direction.x);
               attributes->QueryFloatAttribute("y", &direction.y);
               attributes->QueryFloatAttribute("z", &direction.z);
               am->AddCameraComponent(actor, position, direction);
          }
          else if(strcmp("InputComponent", componentType) == 0)
          {
               am->AddInputComponent(actor);
          }
          
          component = component->NextSiblingElement();
     }
     

     return actor;
}

void Game::Init()
{
     printf("Game Init!\n");

     Frame frame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);
     // Load the shader
     File vertFile = PlatformReadFile("../data/shaders/vert.hlsl", FRAME_MEMORY);
     File fragFile = PlatformReadFile("../data/shaders/frag.hlsl", FRAME_MEMORY);
     shader = GraphicsManager::Get()->ShaderAlloc(vertFile, fragFile);

     MemoryManager::Get()->ReleaseFrame(frame);
     
     // Load the models
     cube.Init("../data/models/cube.obj");
     plane.Init("../data/models/plane.obj");
     pistol.Init("../data/models/pistol.obj");
     sniper.Init("../data/models/sniper.obj");
     warrior.Init("../data/models/warrior.dae");
     wall.Init("../data/models/wall.obj");

     // Load a texture
     texture = GraphicsManager::Get()->TextureAlloc("../data/textures/texture_13.png");
     warriorTexture = GraphicsManager::Get()->TextureAlloc("../data/textures/warrior.png");
     
     // Initialize the Actor Manager
     am.Init(STATIC_MEMORY);

     // Initialize gameplay systems
     inputSystem.Init();
     cameraSystem.Init();
     renderSystem.Init();
     weaponSystem.Init();

     // Register system to be notify
     NotificationManager::Get()->RegisterListener(&weaponSystem, NOTIFICATION_SHOOT);

     // Create sniper
     firstWeapon = am.CreateActor();
     am.AddTransformComponent(firstWeapon, vec3(2.0f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));
     am.AddRenderComponent(firstWeapon, sniper, texture);

     // Create pistol
     secondWeapon = am.CreateActor();
     am.AddTransformComponent(secondWeapon, vec3(1.5f, -0.1f, 0.0f), vec3(0.25f, 0.25f, 0.25f), vec3(0.0f, 0.0f, 1.0f));

     // Create Player
     player = CreateActorFromFile("../data/xml/player.xml", &am);
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


     // Hashmap Test

     HashMap<i32> actorsMap;

     actorsMap.Init(128, STATIC_MEMORY);

     actorsMap.Add("manu", 27);
     actorsMap.Add("tomi", 24);
     actorsMap.Add("mona", 123);
     actorsMap.Add("pepa pig se la come", 69);
     actorsMap.Add("xxx_manu", 27);
     actorsMap.Add("xxx_tomi", 24);

     actorsMap.Remove("mona");
     actorsMap.Remove("manu");

     actorsMap.PrintHashMap();
}

void Game::Update(f32 dt)
{
     inputSystem.Update(&am, dt);
     cameraSystem.Update(&am, dt);
     weaponSystem.Update(&am, dt);

     if(InputManager::Get()->KeyJustDown(KEY_1) && !usingFirstWeapon)
     {
          am.RemoveRenderComponent(secondWeapon);
          am.AddRenderComponent(firstWeapon, sniper, texture);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, firstWeapon);

          usingFirstWeapon = true;

          am.PrintActorAndCompoenentState();
     }
     if(InputManager::Get()->KeyJustDown(KEY_2) && usingFirstWeapon)
     {
          am.RemoveRenderComponent(firstWeapon);
          am.AddRenderComponent(secondWeapon, pistol, texture);
          
          am.RemoveWeaponComponent(player);
          am.AddWeaponComponent(player, secondWeapon);

          usingFirstWeapon = false;

          am.PrintActorAndCompoenentState();
     }
}

void Game::Render()
{    
     GraphicsManager::Get()->BeginFrame(1.0f, 0.0f, 1.0f);

     GraphicsManager::Get()->ShaderBind(shader);

     renderSystem.Render(&am, 0.0f);
     
     GraphicsManager::Get()->EndFrame(1);
}

void Game::Terminate()
{
     inputSystem.Terminate();
     cameraSystem.Terminate();
     renderSystem.Terminate();
     weaponSystem.Terminate();
     
     am.Terminate();

     GraphicsManager::Get()->TextureFree(warriorTexture);
     GraphicsManager::Get()->TextureFree(texture);
     GraphicsManager::Get()->ShaderFree(shader);
     cube.Terminate();
     plane.Terminate();
     pistol.Terminate();
     sniper.Terminate();
     warrior.Terminate();
     wall.Terminate();
     
     printf("Game Terminate!\n");
}
