#include <random>

#include "kinematic_behaviors.h"

f32 RandomFloat() 
{
    static std::random_device rd;   // Seed for random number engine
    static std::mt19937 gen(rd());  // Mersenne Twister engine
    static std::uniform_real_distribution<f32> dist(0.0f, 1.0f);
    return dist(gen);
}

f32 RandomBinomial()
{
    return RandomFloat() - RandomFloat();
}

void AdjustAngle(f32& angle)
{
    f32 PI = pi<f32>();
    while(angle >  (f32)PI) angle -= (f32)(2*PI);
    while(angle < -(f32)PI) angle += (f32)(2*PI);
}

f32 Kinematic::GetNewOrientation(f32 currentOrientation, vec3 velocity)
{
    f32 PI = pi<f32>();
    velocity.y = 0.0f;
    if(length(velocity) > 0.0f)
    {
        velocity = normalize(velocity);
        f32 angle = atan2(-velocity.x, velocity.z);
        if(angle < 0.0f)
        {
            angle += 2*PI;
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
    steering.velocity = normalize(steering.velocity);
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
    steering.velocity = normalize(steering.velocity);
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
    if(length(steering.velocity) < radius)
    {
        return {};
    }
    // We needs to move to our target, we like to
    // get there in timeToTarget seconds
    steering.velocity /= timeToTarget;

    // if its to fast clamp to max speed
    if(length(steering.velocity) > maxSpeed)
    {
        steering.velocity = normalize(steering.velocity);
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
    steering.velocity = maxSpeed * normalize(vec3(-sinf(character->orientation), 0.0f, cosf(character->orientation)));
    steering.velocity.y = 0.0f;
    steering.rotation = RandomBinomial() * maxRotation;
    return steering;
}