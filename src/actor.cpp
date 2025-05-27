#include "actor.h"

#include <components/component.h>
#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/physics_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/anchor_component.h>
#include <components/player_controller_component.h>
#include <components/enemy_component.h>

#include <math/algebra.h>
#include <math/vector3.h>

#include <tinyxml2.h>


i32 ComponentBase::counter = 0;

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

void ActorManager::AllocInternalMemory()
{
     componentsToInit.Init(maxComponentCount, STATIC_MEMORY);
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
     ModelManager *modelManager, MaterialManager* materialManager)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);

    tinyxml2::XMLElement *root = doc.FirstChildElement("Actor");
    
     i32 componentCount = GetChildElementCount(root) + 2; // TODO: handle this better
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
              Vector3 position;
              attributes->QueryFloatAttribute("x", &position.x);
              attributes->QueryFloatAttribute("y", &position.y);
              attributes->QueryFloatAttribute("z", &position.z);
              attributes = attributes->NextSiblingElement();
              Vector3 scale;
              attributes->QueryFloatAttribute("x", &scale.x);
              attributes->QueryFloatAttribute("y", &scale.y);
              attributes->QueryFloatAttribute("z", &scale.z);
              attributes = attributes->NextSiblingElement();
              Vector3 direction;
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
              Vector3 position;
              attributes->QueryFloatAttribute("x", &position.x);
              attributes->QueryFloatAttribute("y", &position.y);
              attributes->QueryFloatAttribute("z", &position.z);
              attributes = attributes->NextSiblingElement();
              Vector3 direction;
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

                    playerController.weapons[0] = CreateActorFromFile(weaponPath[0], modelManager, materialManager); 
                    playerController.weapons[1] = CreateActorFromFile(weaponPath[1], modelManager, materialManager);
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

              const char *materialPath = 0;
              attributes->QueryStringAttribute("name", &materialPath);

              ASSERT(modelPath && materialPath);

              attributes = attributes->NextSiblingElement();

              Vector3 rotationOffset;
              attributes->QueryFloatAttribute("x", &rotationOffset.x);
              attributes->QueryFloatAttribute("y", &rotationOffset.y);
              attributes->QueryFloatAttribute("z", &rotationOffset.z);

              rotationOffset *= (ANT_PI/180.0f);

              RenderComponent render;
              render.model = modelManager->Get(modelPath);
              render.material = materialManager->Get(materialPath);
              render.rotationOffset = rotationOffset;
              AddComponent<RenderComponent>(actor, render);
         }
         else if(strcmp("PhysicsComponent", componentType) == 0)
         {
               tinyxml2::XMLElement *attributes = component->FirstChildElement();
               Vector3 acceleration;
               attributes->QueryFloatAttribute("x", &acceleration.x);
               attributes->QueryFloatAttribute("y", &acceleration.y);
               attributes->QueryFloatAttribute("z", &acceleration.z);
               attributes = attributes->NextSiblingElement();
               Vector3 velocity;
               attributes->QueryFloatAttribute("x", &velocity.x);
               attributes->QueryFloatAttribute("y", &velocity.y);
               attributes->QueryFloatAttribute("z", &velocity.z);
               attributes = attributes->NextSiblingElement();
               Vector3 offset;
               attributes->QueryFloatAttribute("x", &offset.x);
               attributes->QueryFloatAttribute("y", &offset.y);
               attributes->QueryFloatAttribute("z", &offset.z);

               PhysicsComponent physics;
               physics.acceleration = acceleration;
               physics.velocity = velocity;
               physics.forceAccumulator = Vector3(0.0f);
               //physics.lastFrameAcceleration = Vector3(0.0f);
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
               Vector3 offset;
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

void ActorManager::InitializeNewComponents()
{
     for(i32 i = 0; i < componentsToInit.size; ++i)
     {
          componentsToInit[i]->OnInit(this);
          componentsToInit[i]->initialized = true;
     }
     componentsToInit.Clear();
}


