i32 ComponentBase::counter = 0;

template <typename ComponentType>
void ComponentStorage<ComponentType>::RemoveComponent(ActorManager *actorManager, SlotmapKeyBase keyBase)
{
     SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
     ComponentType *component = components.Get(key);
     component->OnTerminate(actorManager);
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
     
     Array<Actor> *actorsArray = actors.GetArray();
     for(i32 i = 0; i < actorsArray->size; ++i)
     {
          Actor *actor = &(*actorsArray)[i];
          for(i32 j = actor->componentsIds.size - 1; j >= 0; --j)
          {
               RemoveComponentById(actor, actor->componentsIds[j]);  
          }
     }
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
     ComponentType *component = GetComponent<ComponentType>(actorKey);
     component->OnTerminate(this);

     ComponentStorage<ComponentType>* compoentStorage = 
          (ComponentStorage<ComponentType> *)(*componentStorageMap.Get(ComponentType::GetID()));

     Actor *actor = GetActor(actorKey);
     SlotmapKeyBase keyBase = *actor->componentsMap.Get(ComponentType::GetID());
     SlotmapKey<ComponentType> key = FromKeyBase<ComponentType>(keyBase);
     ASSERT(key.gen != INVALID_KEY);
     compoentStorage->components.Remove(key);
     actor->componentsMap.Remove(ComponentType::GetID());
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
     compoentStorage->RemoveComponent(this, keyBase);
     actor->componentsMap.Remove(id);
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

void ActorManager::RemoveComponentById(Actor *actor, i32 id)
{
     ComponentStorageBase* compoentStorage = *componentStorageMap.Get(id);
     SlotmapKeyBase keyBase = *actor->componentsMap.Get(id);
     ASSERT(keyBase.gen != INVALID_KEY);
     compoentStorage->RemoveComponent(this, keyBase);
     actor->componentsMap.Remove(id);
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

static i32 NextPower2(u32  x)
{
     int  value  =  1 ;
     while  ( value  <=  x)
     {
          value  =  value  <<  1 ;
     }
     return  value;
}

static i32 GetChildElementCount(tinyxml2::XMLElement* parent) {
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
     TextureManager *textureManager, ModelManager *modelManager)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);

    tinyxml2::XMLElement *root = doc.FirstChildElement("Actor");
    
     i32 componentCount = GetChildElementCount(root);
     if((componentCount & (componentCount - 1)) != 0)
     {
          componentCount = NextPower2(componentCount);
     }

    SlotmapKey<Actor> actor = CreateActor(componentCount);
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
              AddComponent<TransformComponent>(actor, transform);
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
              AddComponent<CameraComponent>(actor, camera);
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

                    playerController.weapons[0] = CreateActorFromFile(weaponPath[0], textureManager, modelManager); 
                    playerController.weapons[1] = CreateActorFromFile(weaponPath[1], textureManager, modelManager);
                    RenderComponent *render = GetComponent<RenderComponent>(playerController.weapons[1]);
                    render->enable = false;
                    WeaponComponent *weapon = GetComponent<WeaponComponent>(actor);
                    weapon->weapon = playerController.weapons[0];
              }
              AddComponent<PlayerControllerComponent>(actor, playerController);
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
              AddComponent<RenderComponent>(actor, render);
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
               attributes = attributes->NextSiblingElement();
               vec3 offset;
               attributes->QueryFloatAttribute("x", &offset.x);
               attributes->QueryFloatAttribute("y", &offset.y);
               attributes->QueryFloatAttribute("z", &offset.z);

               PhysicsComponent physics;
               physics.acceleration = acceleration;
               physics.velocity = velocity;
               physics.forceAccumulator = vec3(0.0f);
               physics.lastFrameAcceleration = vec3(0.0f);
               physics.offset = offset;
               AddComponent(actor, physics);
         }
         else if(strcmp("WeaponComponent", componentType) == 0)
         {
               WeaponComponent weapon;
               AddComponent<WeaponComponent>(actor, weapon);
         }
         else if(strcmp("EnemyComponent", componentType) == 0)
         {
               EnemyComponent enemy;
               AddComponent<EnemyComponent>(actor, enemy);
         }
         else if(strcmp("AnchorComponent", componentType) == 0)
         {
               tinyxml2::XMLElement *attributes = component->FirstChildElement();
               vec3 offset;
               attributes->QueryFloatAttribute("x", &offset.x);
               attributes->QueryFloatAttribute("y", &offset.y);
               attributes->QueryFloatAttribute("z", &offset.z);

               AnchorComponent anchor;
               anchor.offset = offset;
               AddComponent<AnchorComponent>(actor, anchor);
         }
         else
         {
              ASSERT(!"Component not Supported!");
         }
         
         component = component->NextSiblingElement();
    }
    return actor;
}

template<typename ComponentType>
void ActorManager::UpdateComponents(f32 dt)
{
     Array<ComponentType>& components = GetComponents<ComponentType>();
     for(u32 i = 0; i < components.size; ++i)
     {
          if(!components[i].initialized)
          {
               components[i].OnInit(this);
               components[i].initialized = true;
          }

          if(components[i].enable)
          {
               components[i].OnUpdate(this, dt);
          }
     }
}

template<typename ComponentType>
void ActorManager::LateUpdateComponents(f32 dt)
{
     Array<ComponentType>& components = GetComponents<ComponentType>();
     for(u32 i = 0; i < components.size; ++i)
     {
          if(!components[i].initialized)
          {
               components[i].OnInit(this);
               components[i].initialized = true;
          }

          if(components[i].enable)
          {
               components[i].OnLateUpdate(this, dt);
          }
     }
}

template<typename ComponentType>
void ActorManager::RenderComponents(ShaderManager *shaderManager)
{
     Array<ComponentType>& components = GetComponents<ComponentType>();
     for(u32 i = 0; i < components.size; ++i)
     {
          if(!components[i].initialized)
          {
               components[i].OnInit(this);
               components[i].initialized = true;
          }

          if(components[i].enable)
          {
               components[i].OnRender(shaderManager, this);
          }
     }
}
