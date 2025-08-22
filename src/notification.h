#pragma once

#include <math/vector2.h>
#include <math/vector3.h>

enum class NotificationType
{
    Shoot = 0,
    OnResize,
    EnemyHit,

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

struct EnemyHitNotification : public Notification
{
    Actor* enemy;
};