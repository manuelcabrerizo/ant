
void ActorManager::Init(i32 stackNum)
{
     actors.Init(100, stackNum);
     transformComponents.Init(100, stackNum);
     renderComponents.Init(100, stackNum);
     physicsComponents.Init(100, stackNum);
     playerControllerComponents.Init(1, stackNum);
     cameraComponents.Init(1, stackNum);
     weaponComponents.Init(100, stackNum);
}

void ActorManager::Terminate()
{
     actors.Clear();
     transformComponents.Clear();
     renderComponents.Clear();
     physicsComponents.Clear();
     playerControllerComponents.Clear();
     cameraComponents.Clear();
     weaponComponents.Clear();
}

SlotmapKey<Actor> ActorManager::CreateActor()
{
     Actor actor;
     return actors.Add(actor);
}

void ActorManager::DestroyActor(SlotmapKey<Actor> actorKey)
{
     Actor *actor = actors.Get(actorKey);
     if(actor->transform.gen != INVALID_KEY) transformComponents.Remove(actor->transform);
     if(actor->render.gen != INVALID_KEY) renderComponents.Remove(actor->render);
     if(actor->physics.gen != INVALID_KEY) physicsComponents.Remove(actor->physics);
     if(actor->playerController.gen != INVALID_KEY) playerControllerComponents.Remove(actor->playerController);
     if(actor->camera.gen != INVALID_KEY) cameraComponents.Remove(actor->camera);
     if(actor->weapon.gen != INVALID_KEY) weaponComponents.Remove(actor->weapon);
     actors.Remove(actorKey);
}

Actor *ActorManager::GetActor(SlotmapKey<Actor> actorKey)
{
     return actors.Get(actorKey);
}

void ActorManager::AddTransformComponent(SlotmapKey<Actor> actorKey,
                                         vec3 position, vec3 scale, vec3 direction)
{    
     TransformComponent transform;
     transform.position = position;
     transform.scale = scale;
     transform.direction = direction;
     transform.owner = actorKey;
     SlotmapKey<TransformComponent> key = transformComponents.Add(transform);
     Actor *actor = GetActor(actorKey);
     actor->transform = key;
}

void ActorManager::AddRenderComponent(SlotmapKey<Actor> actorKey, Model model, Texture *texture)
{
     RenderComponent render;
     render.model = model;
     render.texture = texture;
     render.owner = actorKey;
     SlotmapKey<RenderComponent> key = renderComponents.Add(render);
     Actor *actor = GetActor(actorKey);
     actor->render = key;
}

void ActorManager::AddPhysicsComponent(SlotmapKey<Actor> actorKey, vec3 acceleration, vec3 velocity)
{
     PhysicsComponent physics;
     physics.acceleration = acceleration;
     physics.velocity = velocity;
     physics.forceAccumulator = vec3(0.0f, 0.0f, 0.0f);
     physics.lastFrameAcceleration = vec3(0.0f, 0.0f, 0.0f);
     physics.owner = actorKey;
     SlotmapKey<PhysicsComponent> key = physicsComponents.Add(physics);
     Actor *actor = GetActor(actorKey);
     actor->physics = key;
}


void ActorManager::AddPlayerControllerComponent(SlotmapKey<Actor> actorKey)
{
     PlayerControllerComponent playerController;
     playerController.owner = actorKey;
     SlotmapKey<PlayerControllerComponent> key = playerControllerComponents.Add(playerController);
     Actor *actor = GetActor(actorKey);
     actor->playerController = key;
}

void ActorManager::AddCameraComponent(SlotmapKey<Actor> actorKey, vec3 pos, vec3 dir)
{
     CameraComponent camera;
     camera.Init(pos, dir);
     camera.owner = actorKey;
     SlotmapKey<CameraComponent> key = cameraComponents.Add(camera);
     Actor *actor = GetActor(actorKey);
     actor->camera = key;
}

void ActorManager::AddWeaponComponent(SlotmapKey<Actor> actorKey, SlotmapKey<Actor> weaponKey)
{
     WeaponComponent weapon;
     weapon.weapon = weaponKey;
     weapon.owner = actorKey;
     SlotmapKey<WeaponComponent> key = weaponComponents.Add(weapon);
     Actor *actor = GetActor(actorKey);
     actor->weapon = key;
}

#define GetComponentsImp(type, slotmap) \
Array<type>& ActorManager::Get##type##s() \
{ \
     return *slotmap.GetArray(); \
}

GetComponentsImp(TransformComponent, transformComponents);
GetComponentsImp(RenderComponent, renderComponents);
GetComponentsImp(PhysicsComponent, physicsComponents);
GetComponentsImp(PlayerControllerComponent, playerControllerComponents);
GetComponentsImp(CameraComponent, cameraComponents);
GetComponentsImp(WeaponComponent, weaponComponents);


#define RemoveComponentImp(type, slotmapKey, slotmap) \
void ActorManager::Remove##type(SlotmapKey<Actor> actorKey) \
{ \
     Actor *actor = GetActor(actorKey); \
     if(actor->slotmapKey.gen != INVALID_KEY) slotmap.Remove(actor->slotmapKey); \
}

RemoveComponentImp(TransformComponent, transform, transformComponents);
RemoveComponentImp(RenderComponent, render, renderComponents);
RemoveComponentImp(PhysicsComponent, physics, physicsComponents);
RemoveComponentImp(PlayerControllerComponent, playerController, playerControllerComponents);
RemoveComponentImp(CameraComponent, camera, cameraComponents);
RemoveComponentImp(WeaponComponent, weapon, weaponComponents);


#define GetComponentImp(type, slotmapKey, slotmap) \
type *ActorManager::Get##type(SlotmapKey<Actor> actorKey) \
{ \
     Actor *actor = GetActor(actorKey); \
     return slotmap.Get(actor->slotmapKey); \
}

GetComponentImp(TransformComponent, transform, transformComponents);
GetComponentImp(RenderComponent, render, renderComponents);
GetComponentImp(PhysicsComponent, physics, physicsComponents);
GetComponentImp(PlayerControllerComponent, playerController, playerControllerComponents);
GetComponentImp(CameraComponent, camera, cameraComponents);
GetComponentImp(WeaponComponent, weapon, weaponComponents);

void ActorManager::PrintActorAndCompoenentState()
{
     printf("Actors count: %d\n", actors.Size());
     printf("transformComponents count: %d\n", transformComponents.Size());
     printf("renderComponents count: %d\n", renderComponents.Size());
     printf("playerComponents count: %d\n", playerControllerComponents.Size());
     printf("cameraComponents count: %d\n", cameraComponents.Size());
     printf("weaponComponents count: %d\n", weaponComponents.Size());
}

// TODO: Integrate this function int the engine
SlotmapKey<Actor> CreateActorFromFile(const char *filepath,
     ActorManager *actorManager, TextureManager *textureManager, ModelManager *modelManager)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);

    SlotmapKey<Actor> actor = actorManager->CreateActor();

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
              actorManager->AddTransformComponent(actor, position, scale, direction);
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
              actorManager->AddCameraComponent(actor, position, direction);
         }
         else if(strcmp("PlayerControllerComponent", componentType) == 0)
         {
              actorManager->AddPlayerControllerComponent(actor);
         }
         else if(strcmp("RenderComponent", componentType) == 0)
         {
              tinyxml2::XMLElement *attributes = component->FirstChildElement();

              const char *modelPath = 0;
              attributes->QueryStringAttribute("name", &modelPath);

              attributes = attributes->NextSiblingElement();

              const char *texturePath = 0;
              attributes->QueryStringAttribute("name", &texturePath);

              ASSERT(modelPath && texturePath);

              ModelHandle *modelHandle = modelManager->Get(modelPath);
              TextureHandle *textureHandle = textureManager->Get(texturePath);
              actorManager->AddRenderComponent(actor, modelHandle->model, textureHandle->texture);
         }
         else
         {
              ASSERT(!"Component not Supported!");
         }
         
         component = component->NextSiblingElement();
    }
    return actor;
}

