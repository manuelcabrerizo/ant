#pragma once

#include <common.h>
#include <math/vector3.h>

f32 RandomFloat();
f32 RandomBinomial();
void AdjustAngle(f32& angle);

struct KinematicSteeringOutput
{
    Vector3 velocity = Vector3(0.0f);
    f32 rotation = 0.0f;
};

struct Kinematic
{
    Vector3 position = Vector3(0.0f);
    Vector3 velocity = Vector3(0.0f);
    f32 orientation = 0.0f;
    f32 rotation = 0.0f;
    static f32 GetNewOrientation(f32 currentOrientation, Vector3 velocity);
};

class KinematicSeek
{
private:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxSpeed = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxSpeed(f32 maxSpeed);
    KinematicSteeringOutput GetSteering();
};

class KinematicFlee
{
private:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxSpeed = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxSpeed(f32 maxSpeed);
    KinematicSteeringOutput GetSteering();
};

class KinematicArrive
{
private:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxSpeed = 0.0f;
    f32 radius = 0.0f;
    f32 timeToTarget = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxSpeed(f32 maxSpeed);
    void SetRadius(f32 radius);
    void SetTimeToTarget(f32 timeToTarget);
    KinematicSteeringOutput GetSteering();
};

class KinematicWander
{
private:
    Kinematic *character = nullptr;
    f32 maxSpeed = 0.0f;
    f32 maxRotation = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetMaxSpeed(f32 maxSpeed);
    void SetMaxRotation(f32 maxRotation);
    KinematicSteeringOutput GetSteering();
};
