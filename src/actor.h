#pragma once

class ActorManager;

struct Actor
{
     HashMap<SlotmapKeyBase> componentsMap;
     Array<i32> componentsIds;
};

struct ComponentStorageBase
{
     virtual void RemoveComponent(ActorManager *actorManager, SlotmapKeyBase keyBase) = 0;
};

template <typename ComponentType>
struct ComponentStorage : ComponentStorageBase
{
     Slotmap<ComponentType> components;
     void RemoveComponent(ActorManager *actorManager, SlotmapKeyBase keyBase) override;
};

class ActorManager
{
private:
     i32 memoryType = STATIC_MEMORY;
     i32 componentCount = 0;
     i32 maxActorCount = 0;

     Slotmap<Actor> actors;
     HashMap<ComponentStorageBase *> componentStorageMap;     
public:
     void Init(i32 actorCount, i32 componentCount, i32 memoryType);
     void Terminate();

     template <typename ComponentType, i32 Count>
     void AddComponentType();

     template <typename ComponentType>
     Array<ComponentType>& GetComponents();

     SlotmapKey<Actor> CreateActor(i32 componentCount);
     SlotmapKey<Actor> CreateActorFromFile(const char *filepath, TextureManager *textureManager, ModelManager *modelManager);
     void DestroyActor(SlotmapKey<Actor> actorKey);
     Actor *GetActor(SlotmapKey<Actor> actorKey);
    
     template <typename ComponentType>
     void AddComponent(SlotmapKey<Actor> actorKey, ComponentType component);

     template <typename ComponentType>
     void RemoveComponent(SlotmapKey<Actor> actorKey);
     void RemoveComponentById(SlotmapKey<Actor> actorKey, i32 id);
     void RemoveComponentById(Actor *actor, i32 id);
     
     template <typename ComponentType>
     ComponentType *GetComponent(SlotmapKey<Actor> actorKey);

     template<typename ComponentType>
     void UpdateComponents(f32 dt);

     template<typename ComponentType>
     void LateUpdateComponents(f32 dt);

     template<typename ComponentType>
     void RenderComponents(ShaderManager *shaderManager);
};
