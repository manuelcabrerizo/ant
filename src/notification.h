#pragma once

#include <math/vector2.h>

enum class NotificationType
{
    Shoot = 0,
    OnResize,

    // last item
    Count
};

struct Notification { };

struct ShootNotification : public Notification
{
};

struct OnResizeNotification : public Notification
{
    Vector2 extent;
};