#include "kinematic_behaviors.h"
#include <math/algebra.h>
#include <utils.h>


f32 RandomBinomial()
{
    return Utils::RandRange(0.0f, 1.0f) - Utils::RandRange(0.0f, 1.0f);
}

void AdjustAngle(f32& angle)
{
    while(angle >  ANT_PI) angle -= (f32)(ANT_TAU);
    while(angle < -ANT_PI) angle += (f32)(ANT_TAU);
}

f32 Kinematic::GetNewOrientation(f32 currentOrientation, Vector3 velocity)
{
    velocity.y = 0.0f;
    if(velocity.Magnitude() > 0.0f)
    {
        velocity.Normalize();
        f32 angle = atan2(-velocity.x, velocity.z);
        if(angle < 0.0f)
        {
            angle += ANT_TAU;
        }
        return angle;
    }
    else
    {
        return currentOrientation;
    }
}

void KinematicSeek::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void KinematicSeek::SetTarget(Kinematic *target)
{
    this->target = target;
}

void KinematicSeek::SetMaxSpeed(f32 maxSpeed)
{
    this->maxSpeed = maxSpeed;
}

KinematicSteeringOutput KinematicSeek::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    KinematicSteeringOutput steering;
    // Get the direction to the target
    steering.velocity = target->position - character->position;
    steering.velocity.y = 0.0f;
    // The velocity is along this direction, at full speed
    steering.velocity.Normalize();
    steering.velocity *= maxSpeed;
    character->orientation = Kinematic::GetNewOrientation(character->orientation, steering.velocity);

    steering.rotation = 0.0f;

    return steering;
}


void KinematicFlee::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void KinematicFlee::SetTarget(Kinematic *target)
{
    this->target = target;
}

void KinematicFlee::SetMaxSpeed(f32 maxSpeed)
{
    this->maxSpeed = maxSpeed;
}

KinematicSteeringOutput KinematicFlee::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    KinematicSteeringOutput steering;
    // Get the oposite direction to the target
    steering.velocity = character->position - target->position;
    steering.velocity.y = 0.0f;
    // The velocity is along this direction, at full speed
    steering.velocity.Normalize();
    steering.velocity *= maxSpeed;
    character->orientation = Kinematic::GetNewOrientation(character->orientation, steering.velocity);

    steering.rotation = 0.0f;

    return steering;
}


void KinematicArrive::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void KinematicArrive::SetTarget(Kinematic *target)
{
    this->target = target;
}

void KinematicArrive::SetMaxSpeed(f32 maxSpeed)
{
    this->maxSpeed = maxSpeed;
}

void KinematicArrive::SetRadius(f32 radius)
{
    this->radius = radius;
}

void KinematicArrive::SetTimeToTarget(f32 timeToTarget)
{
    this->timeToTarget = timeToTarget;
}

KinematicSteeringOutput KinematicArrive::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    KinematicSteeringOutput steering;
    // Get the direction to the target
    steering.velocity = target->position - character->position;
    steering.velocity.y = 0.0f;
    if(steering.velocity.Magnitude() < radius)
    {
        return {};
    }
    // We needs to move to our target, we like to
    // get there in timeToTarget seconds
    steering.velocity /= timeToTarget;

    // if its to fast clamp to max speed
    if(steering.velocity.Magnitude() > maxSpeed)
    {
        steering.velocity.Normalize();
        steering.velocity *= maxSpeed;
    }

    character->orientation = Kinematic::GetNewOrientation(character->orientation, steering.velocity);

    steering.rotation = 0.0f;

    return steering;
}

void KinematicWander::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void KinematicWander::SetMaxSpeed(f32 maxSpeed)
{
    this->maxSpeed = maxSpeed;
}

void KinematicWander::SetMaxRotation(f32 maxRotation)
{
    this->maxRotation = maxRotation;
}

KinematicSteeringOutput KinematicWander::GetSteering()
{
    ASSERT(character != nullptr);
    KinematicSteeringOutput steering;
    steering.velocity = Vector3(-sinf(character->orientation), 0.0f, cosf(character->orientation)).Normalized() * maxSpeed;
    steering.velocity.y = 0.0f;
    steering.rotation = RandomBinomial() * maxRotation;
    return steering;
}