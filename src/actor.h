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

// TODO: change the name to PlayerControllerComponent
struct InputComponent
{
     f32 yaw, pitch;
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
     SlotmapKey<InputComponent> input;
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
     Slotmap<InputComponent> inputComponents;
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
     Array<InputComponent>& GetInputComponents();
     Array<CameraComponent>& GetCameraComponents();
     Array<WeaponComponent>& GetWeaponComponents();
     
     void AddTransformComponent(SlotmapKey<Actor> actorKey,
                                vec3 position, vec3 scale, vec3 direction);
     void AddRenderComponent(SlotmapKey<Actor> actorKey, Model model, Texture *texture);
     void AddPhysicsComponent(SlotmapKey<Actor> actorKey, vec3 acceleration, vec3 velocity);
     void AddInputComponent(SlotmapKey<Actor> actorKey);
     void AddCameraComponent(SlotmapKey<Actor> actorKey, vec3 pos, vec3 dir);
     void AddWeaponComponent(SlotmapKey<Actor> actorKey, SlotmapKey<Actor> weaponKey);

     void RemoveTransformComponent(SlotmapKey<Actor> actorKey);
     void RemoveRenderComponent(SlotmapKey<Actor> actorKey);
     void RemovePhysicsComponent(SlotmapKey<Actor> actorKey);
     void RemoveInputComponent(SlotmapKey<Actor> actorKey);
     void RemoveCameraComponent(SlotmapKey<Actor> actorKey);
     void RemoveWeaponComponent(SlotmapKey<Actor> actorKey);
     
     TransformComponent *GetTransformComponent(SlotmapKey<Actor> actorKey);
     RenderComponent *GetRenderComponent(SlotmapKey<Actor> actorKey);
     PhysicsComponent *GetPhysicsComponent(SlotmapKey<Actor> actorKey);
     InputComponent *GetInputComponent(SlotmapKey<Actor> actorKey);
     CameraComponent *GetCameraComponent(SlotmapKey<Actor> actorKey);
     WeaponComponent *GetWeaponComponent(SlotmapKey<Actor> actorKey);

     void PrintActorAndCompoenentState();
};
