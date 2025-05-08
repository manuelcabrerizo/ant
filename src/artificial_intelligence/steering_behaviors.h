#pragma once

#include "kinematic_behaviors.h"

struct SteeringOutput
{
    Vector3 linear = Vector3(0.0f);
    f32 angular = 0.0f;
};

class SteeringSeek
{
protected:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxAcceleration = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxAcceleration(f32 maxAcceleration);
    SteeringOutput GetSteering();
};

class SteeringFlee
{
protected:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxAcceleration = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxAcceleration(f32 maxAcceleration);
    SteeringOutput GetSteering();
};

class SteeringArrive
{
protected:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxAcceleration = 0.0f;
    f32 maxSpeed = 0.0f;
    f32 targetRadius = 0.0f;
    f32 slowRadius = 0.0f;
    f32 timeToTarget = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxAcceleration(f32 maxAcceleration);
    void SetMaxSpeed(f32 maxSpeed);
    void SetTargetRadius(f32 targetRadius);
    void SetSlowRadius(f32 slowRadius);
    void SetTimeToTarget(f32 timeToTarget);
    SteeringOutput GetSteering();
};

class SteeringAlign
{
protected:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxAngularAcceleration = 0.0f;
    f32 maxRotation = 0.0f;
    f32 targetRadius = 0.0f;
    f32 slowRadius = 0.0f;
    f32 timeToTarget = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxAngularAcceleration(f32 maxAngularAcceleration);
    void SetMaxRotation(f32 maxRotation);
    void SetTargetRadius(f32 targetRadius);
    void SetSlowRadius(f32 slowRadius);
    void SetTimeToTarget(f32 timeToTarget);
    SteeringOutput GetSteering();
};

class SteeringVelocityMach
{
protected:
    Kinematic *character = nullptr;
    Kinematic *target = nullptr;
    f32 maxAcceleration = 0.0f;
    f32 timeToTarget = 0.0f;
public:
    void SetCharacter(Kinematic *character);
    void SetTarget(Kinematic *target);
    void SetMaxAcceleration(f32 maxAcceleration);
    void SetTimeToTarget(f32 timeToTarget);
    SteeringOutput GetSteering();
};

class SteeringPursue : public SteeringArrive
{
private:
    f32 maxPrediction = 0.01f;
public:
    void SetMaxPrediction(f32 maxPrediction);
    SteeringOutput GetSteering();
};

class SteeringFace : public SteeringAlign
{
public:
    SteeringOutput GetSteering();
};

class SteeringLookWhereYoureGoing : public SteeringAlign
{
public:
    SteeringOutput GetSteering();
};

class SteeringWander : public SteeringFace
{
private:
    f32 wanderOffset = 0.0f;
    f32 wanderRadius = 0.0f;
    f32 wanderRate = 0.0f;
    f32 wanderOrientation = 0.0f;
    f32 maxAcceleration = 0.0f;
public:
    void SetWanderOffset(f32 wanderOffset);
    void SetWanderRadius(f32 wanderRadius);
    void SetWanderRate(f32 wanderRate);
    void SetWanderOrientation(f32 wanderOrientation);
    void SetMaxAcceleration(f32 maxAcceleration);
    SteeringOutput GetSteering();
};