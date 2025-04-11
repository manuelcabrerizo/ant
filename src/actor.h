#pragma once

struct Actor;
class ActorManager;

// TODO: test the bug here

struct Component
{
     bool enable = true;
};

struct TransformComponent : public Component
{
     vec3 position;
     vec3 scale;
     vec3 direction; // TODO: use quaternioonr for orientation
     SlotmapKey<Actor> owner;
};

struct RenderComponent : public Component
{
     Texture *texture;
     Model model;
     //bool enable = true;
     SlotmapKey<Actor> owner;
};

struct PhysicsComponent : public Component
{
     vec3 acceleration;
     vec3 velocity;
     vec3 forceAccumulator;
     vec3 lastFrameAcceleration;
     SlotmapKey<Actor> owner;
};

struct PlayerControllerComponent : public Component
{
     f32 yaw = 0;
     f32 pitch = 0;
     bool grounded = false;
     SlotmapKey<Actor> weapons[2];
     i32 weaponCount = 0;
     bool usingFirstWeapon = true;
     SlotmapKey<Actor> owner;
};

struct WeaponComponent : public Component
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
