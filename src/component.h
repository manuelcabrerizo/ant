struct Actor;

struct ComponentBase
{
    static i32 counter;
    bool enable = true;
    SlotmapKey<Actor> owner;
};

template <typename>
struct Component : public ComponentBase
{
    static i32 GetID()
    {
        static i32 id = ++counter;
        return id;
    }
};

struct TransformComponent : public Component<TransformComponent>
{
    vec3 position;
    vec3 scale;
    vec3 direction; // TODO: use quaternioonr for orientation
};

struct RenderComponent : public Component<RenderComponent>
{
    Texture *texture;
    Model *model;
};

struct PhysicsComponent : public Component<PhysicsComponent>
{
    vec3 acceleration;
    vec3 velocity;
    vec3 forceAccumulator;
    vec3 lastFrameAcceleration;
    vec3 offset;
    bool grounded = false;
};

struct PlayerControllerComponent : public Component<PlayerControllerComponent>
{
    f32 yaw = 0;
    f32 pitch = 0;
    SlotmapKey<Actor> weapons[2];
    i32 weaponCount = 0;
    bool usingFirstWeapon = true;
};

struct WeaponComponent : public Component<WeaponComponent>
{
    SlotmapKey<Actor> weapon;
};

struct EnemyComponent : public Component<EnemyComponent>
{
};