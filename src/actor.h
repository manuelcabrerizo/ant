#pragma once

struct Actor;
class ActorManager;

struct TransformComponent
{
     vec3 position;
     vec3 scale;
     vec3 direction; // TODO: use quaternioonr for orientation
     SlotmapKey<Actor> owner;
};

struct RenderComponent
{
     Texture *texture;
     Model model;
     SlotmapKey<Actor> owner;
};

struct PhysicsComponent
{
     vec3 acceleration;
     vec3 velocity;
     vec3 forceAccumulator;
     vec3 lastFrameAcceleration;
     SlotmapKey<Actor> owner;
};

struct PlayerControllerComponent
{
     f32 yaw, pitch;
     bool grounded = false;
     SlotmapKey<Actor> owner;
};

struct WeaponComponent
{
     SlotmapKey<Actor> owner;
     SlotmapKey<Actor> weapon;
};

struct Actor
{
     SlotmapKey<TransformComponent> transform;
     SlotmapKey<RenderComponent> render;
     SlotmapKey<PhysicsComponent> physics;
     SlotmapKey<PlayerControllerComponent> playerController;
     SlotmapKey<CameraComponent> camera;
     SlotmapKey<WeaponComponent> weapon;
};

class ActorManager
{
private:
     Slotmap<Actor> actors;
     Slotmap<TransformComponent> transformComponents;
     Slotmap<RenderComponent> renderComponents;
     Slotmap<PhysicsComponent> physicsComponents;
     Slotmap<PlayerControllerComponent> playerControllerComponents;
     Slotmap<CameraComponent> cameraComponents;
     Slotmap<WeaponComponent> weaponComponents;
     
public:
     void Init(i32 stackNum);
     void Terminate();

     SlotmapKey<Actor> CreateActor();
     void DestroyActor(SlotmapKey<Actor> actorKey);

     Actor *GetActor(SlotmapKey<Actor> actorKey);
     // TODO: return a reference not a pointer     
     Array<TransformComponent>& GetTransformComponents();
     Array<RenderComponent>& GetRenderComponents();
     Array<PhysicsComponent>& GetPhysicsComponents();
     Array<PlayerControllerComponent>& GetPlayerControllerComponents();
     Array<CameraComponent>& GetCameraComponents();
     Array<WeaponComponent>& GetWeaponComponents();
     
     void AddTransformComponent(SlotmapKey<Actor> actorKey,
                                vec3 position, vec3 scale, vec3 direction);
     void AddRenderComponent(SlotmapKey<Actor> actorKey, Model model, Texture *texture);
     void AddPhysicsComponent(SlotmapKey<Actor> actorKey, vec3 acceleration, vec3 velocity);
     void AddPlayerControllerComponent(SlotmapKey<Actor> actorKey);
     void AddCameraComponent(SlotmapKey<Actor> actorKey, vec3 pos, vec3 dir);
     void AddWeaponComponent(SlotmapKey<Actor> actorKey, SlotmapKey<Actor> weaponKey);

     void RemoveTransformComponent(SlotmapKey<Actor> actorKey);
     void RemoveRenderComponent(SlotmapKey<Actor> actorKey);
     void RemovePhysicsComponent(SlotmapKey<Actor> actorKey);
     void RemovePlayerControllerComponent(SlotmapKey<Actor> actorKey);
     void RemoveCameraComponent(SlotmapKey<Actor> actorKey);
     void RemoveWeaponComponent(SlotmapKey<Actor> actorKey);
     
     TransformComponent *GetTransformComponent(SlotmapKey<Actor> actorKey);
     RenderComponent *GetRenderComponent(SlotmapKey<Actor> actorKey);
     PhysicsComponent *GetPhysicsComponent(SlotmapKey<Actor> actorKey);
     PlayerControllerComponent *GetPlayerControllerComponent(SlotmapKey<Actor> actorKey);
     CameraComponent *GetCameraComponent(SlotmapKey<Actor> actorKey);
     WeaponComponent *GetWeaponComponent(SlotmapKey<Actor> actorKey);

     void PrintActorAndCompoenentState();
};
