i32 ComponentBase::counter = 0;

template <typename ComponentType>
void ComponentStorage<ComponentType>::RemoveComponent(SlotmapKeyBase keyBase)
{
     SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
     components.Remove(key);
}

void ActorManager::Init(i32 actorCount, i32 componentCount_, i32 memoryType_)
{
     memoryType = memoryType_;
     componentCount = componentCount_;
     maxActorCount = actorCount;

     actors.Init(maxActorCount, memoryType);
     componentStorageMap.Init(componentCount, memoryType);
}

void ActorManager::Terminate()
{
     actors.Clear();
}

template <typename ComponentType, i32 Count>
void ActorManager::AddComponentType()
{
     void *buffer = MemoryManager::Get()->Alloc(sizeof(ComponentStorage<ComponentType>), memoryType);
     ComponentStorage<ComponentType> *componentStorage = new (buffer) ComponentStorage<ComponentType>();
     componentStorage->components.Init(Count, memoryType);
     componentStorageMap.Add(ComponentType::GetID(), (ComponentStorageBase *)componentStorage);
}

template <typename ComponentType>
Array<ComponentType>& ActorManager::GetComponents()
{
     ComponentStorage<ComponentType>* compoentStorage = 
          (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));
     return *compoentStorage->components.GetArray();
}


SlotmapKey<Actor> ActorManager::CreateActor(i32 componentCount_)
{
     Actor actor;
     actor.componentsMap.Init(componentCount_, memoryType);
     actor.componentsIds.Init(componentCount_, memoryType);
     SlotmapKey<Actor> actorKey = actors.Add(actor); 
     return actorKey;
}

void ActorManager::DestroyActor(SlotmapKey<Actor> actorKey)
{
     Actor *actor = actors.Get(actorKey);
     for(i32 i = actor->componentsIds.size - 1; i >= 0; --i)
     {
          RemoveComponentById(actorKey, actor->componentsIds[i]);  
     }
     actor->componentsIds.Clear();
     actors.Remove(actorKey);
}

Actor *ActorManager::GetActor(SlotmapKey<Actor> actorKey)
{
     return actors.Get(actorKey);
}

template <typename ComponentType>
void ActorManager::AddComponent(SlotmapKey<Actor> actorKey, ComponentType component)
{
     component.owner = actorKey;

     ComponentStorage<ComponentType>* compoentStorage = 
          (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

     SlotmapKey<ComponentType> key = compoentStorage->components.Add(component);

     Actor *actor = GetActor(actorKey);
     SlotmapKeyBase keyBase = FromKey<ComponentType>(key);
     actor->componentsMap.Add(ComponentType::GetID(), keyBase);
     actor->componentsIds.Push(ComponentType::GetID());
} 

template <typename ComponentType>
void ActorManager::RemoveComponent(SlotmapKey<Actor> actorKey)
{
     ComponentStorage<ComponentType>* compoentStorage = 
          (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

     Actor *actor = GetActor(actorKey);
     SlotmapKeyBase keyBase = *actor->componentsMap.Get(ComponentType::GetID());
     SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
     ASSERT(key.gen != INVALID_KEY);
     compoentStorage->components.Remove(key);
     actor->componentsMap.Remove(ComponentType::GetID());

     // TODO: try to find a way of not having this linear search
     i32 foundIndex = -1;
     for(i32 i = 0; i < actor->componentsIds.size; ++i)
     {
          if(actor->componentsIds[i] == ComponentType::GetID())
          {
               foundIndex = i;
          }
     }
     if(foundIndex >= 0)
     {
          actor->componentsIds[foundIndex] = actor->componentsIds[actor->componentsIds.size - 1];
          actor->componentsIds[actor->componentsIds.size - 1] = 0;
          actor->componentsIds.size--;
     }
}

void ActorManager::RemoveComponentById(SlotmapKey<Actor> actorKey, i32 id)
{
     ComponentStorageBase* compoentStorage = *componentStorageMap.Get(id);
     Actor *actor = GetActor(actorKey);
     SlotmapKeyBase keyBase = *actor->componentsMap.Get(id);
     ASSERT(keyBase.gen != INVALID_KEY);
     compoentStorage->RemoveComponent(keyBase);
     actor->componentsMap.Remove(id);

     // TODO: try to find a way of not having this linear search
     i32 foundIndex = -1;
     for(i32 i = 0; i < actor->componentsIds.size; ++i)
     {
          if(actor->componentsIds[i] == id)
          {
               foundIndex = i;
          }
     }
     if(foundIndex >= 0)
     {
          actor->componentsIds[foundIndex] = actor->componentsIds[actor->componentsIds.size - 1];
          actor->componentsIds[actor->componentsIds.size - 1] = 0;
          actor->componentsIds.size--;
     }
}

template <typename ComponentType>
ComponentType *ActorManager::GetComponent(SlotmapKey<Actor> actorKey)
{
     ComponentStorage<ComponentType>* compoentStorage = 
          (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

     Actor *actor = GetActor(actorKey);
     SlotmapKeyBase keyBase = *actor->componentsMap.Get(ComponentType::GetID());
     SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
     ASSERT(key.gen != INVALID_KEY);
     return compoentStorage->components.Get(key);
}

i32 NextPower2(u32  x)
{
     int  value  =  1 ;
     while  ( value  <=  x)
     {
          value  =  value  <<  1 ;
     }
     return  value;
}

i32 GetChildElementCount(tinyxml2::XMLElement* parent) {
     int count = 0;
     for (tinyxml2::XMLElement* child = parent->FirstChildElement();
          child != nullptr;
          child = child->NextSiblingElement())
     {
         ++count;
     }
     return count;
 }

SlotmapKey<Actor> ActorManager::CreateActorFromFile(const char *filepath,
     ActorManager *actorManager, TextureManager *textureManager, ModelManager *modelManager)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);

    tinyxml2::XMLElement *root = doc.FirstChildElement("Actor");
    
     i32 componentCount = GetChildElementCount(root);
     if((componentCount & (componentCount - 1)) != 0)
     {
          componentCount = NextPower2(componentCount);
     }

    SlotmapKey<Actor> actor = actorManager->CreateActor(componentCount);
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
              TransformComponent transform;
              transform.position = position;
              transform.scale = scale;
              transform.direction = direction;
              actorManager->AddComponent<TransformComponent>(actor, transform);
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
              CameraComponent camera;
              camera.Init(position, direction);
              actorManager->AddComponent<CameraComponent>(actor, camera);
         }
         else if(strcmp("PlayerControllerComponent", componentType) == 0)
         {
               PlayerControllerComponent playerController;
              // TODO: make this work for variable amount of weapons
              tinyxml2::XMLElement *attributes = component->FirstChildElement();
              if(attributes)
              {
                    const char *weaponPath[2];
                    attributes->QueryStringAttribute("path", &weaponPath[0]);
                    attributes = attributes->NextSiblingElement();
                    attributes->QueryStringAttribute("path", &weaponPath[1]);

                    playerController.weapons[0] = CreateActorFromFile(weaponPath[0], actorManager, textureManager, modelManager); 
                    playerController.weapons[1] = CreateActorFromFile(weaponPath[1], actorManager, textureManager, modelManager);
                    RenderComponent *render = actorManager->GetComponent<RenderComponent>(playerController.weapons[1]);
                    render->enable = false;
                    WeaponComponent *weapon = actorManager->GetComponent<WeaponComponent>(actor);
                    weapon->weapon = playerController.weapons[0];
              }
              actorManager->AddComponent<PlayerControllerComponent>(actor, playerController);

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

              RenderComponent render;
              render.model = modelManager->Get(modelPath);
              render.texture = textureManager->Get(texturePath);
              actorManager->AddComponent<RenderComponent>(actor, render);
         }
         else if(strcmp("PhysicsComponent", componentType) == 0)
         {
               tinyxml2::XMLElement *attributes = component->FirstChildElement();
               vec3 acceleration;
               attributes->QueryFloatAttribute("x", &acceleration.x);
               attributes->QueryFloatAttribute("y", &acceleration.y);
               attributes->QueryFloatAttribute("z", &acceleration.z);
               attributes = attributes->NextSiblingElement();
               vec3 velocity;
               attributes->QueryFloatAttribute("x", &velocity.x);
               attributes->QueryFloatAttribute("y", &velocity.y);
               attributes->QueryFloatAttribute("z", &velocity.z);

               PhysicsComponent physics;
               physics.acceleration = acceleration;
               physics.velocity = velocity;
               physics.forceAccumulator = vec3(0.0f);
               physics.lastFrameAcceleration = vec3(0.0f);
               actorManager->AddComponent(actor, physics);
         }
         else if(strcmp("WeaponComponent", componentType) == 0)
         {
               WeaponComponent weapon;
               actorManager->AddComponent<WeaponComponent>(actor, weapon);
         }
         else if(strcmp("EnemyComponent", componentType) == 0)
         {
               EnemyComponent enemy;
               actorManager->AddComponent<EnemyComponent>(actor, enemy);
         }
         else
         {
              ASSERT(!"Component not Supported!");
         }
         
         component = component->NextSiblingElement();
    }
    return actor;
}
