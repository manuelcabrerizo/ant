#pragma once

#include <math/vector2.h>
#include <math/vector3.h>

enum class NotificationType
{
    Shoot = 0,
    OnResize,
    EnemyHit,
    PlayerMove,
    Signal,
    PlayerWin,
    EnemySpawn,
    EnemyKill,
    AmmoChange,
    PlayerLifeChange,
    PlayerHit,

    // last item
    Count
};

struct Notification { };

struct ShootNotification : public Notification
{
    Vector3 shootPosition;
    Vector3 shootDirection;
};

struct OnResizeNotification : public Notification
{
    Vector2 extent;
};

class Actor;
class BloodParticleSystem;

struct EnemyHitNotification : public Notification
{
    Actor* enemy;
    BloodParticleSystem* ps;
    float* bloodTimer;
    float bloodDuration;
};

class TransformComponent;

struct PlayerMoveNotification : public Notification
{
    TransformComponent* transform;
    Vector3 position;
    Vector3 velocity;
};

class ISignable;

struct SignalNotification : public Notification
{
    ISignable* signable;
};

struct PlayerWinNotification : public Notification
{
};

struct EnemySpawnNotification : public Notification
{
};

struct EnemyKillNotification : public Notification
{
};

struct AmmoChangeNotification : public Notification
{
    int maxAmmo;
    int currentAmmo;
};

struct PlayerLifeChangeNotification : public Notification
{
    int maxLife;
    int life;
};

struct PlayerHitNotification : public Notification
{
    Actor* actor;
    int hitAmount;
};