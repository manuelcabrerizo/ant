#include "steering_behaviors.h"

void SteeringSeek::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void SteeringSeek::SetTarget(Kinematic *target)
{
    this->target = target;
}

void SteeringSeek::SetMaxAcceleration(f32 maxAcceleration)
{
    this->maxAcceleration = maxAcceleration;
}

SteeringOutput SteeringSeek::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    SteeringOutput steering;
    // Get the direction to the target
    steering.linear = target->position - character->position;
    steering.linear.y = 0.0f;
    // Give full acceleration along this direction
    steering.linear = normalize(steering.linear);
    steering.linear *= maxAcceleration;
    steering.angular = 0.0f;
    return steering;
}

void SteeringFlee::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void SteeringFlee::SetTarget(Kinematic *target)
{
    this->target = target;
}

void SteeringFlee::SetMaxAcceleration(f32 maxAcceleration)
{
    this->maxAcceleration = maxAcceleration;
}

SteeringOutput SteeringFlee::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    SteeringOutput steering;
    // Get the direction to the target
    steering.linear = character->position - target->position;
    steering.linear.y = 0.0f;
    // Give full acceleration along this direction
    steering.linear = normalize(steering.linear);
    steering.linear *= maxAcceleration;
    steering.angular = 0.0f;
    return steering;
}

void SteeringArrive::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void SteeringArrive::SetTarget(Kinematic *target)
{
    this->target = target;
}

void SteeringArrive::SetMaxAcceleration(f32 maxAcceleration)
{
    this->maxAcceleration = maxAcceleration;
}

void SteeringArrive::SetMaxSpeed(f32 maxSpeed)
{
    this->maxSpeed = maxSpeed;
}

void SteeringArrive::SetTargetRadius(f32 targetRadius)
{
    this->targetRadius = targetRadius;
}

void SteeringArrive::SetSlowRadius(f32 slowRadius)
{
    this->slowRadius = slowRadius;
}

void SteeringArrive::SetTimeToTarget(f32 timeToTarget)
{
    this->timeToTarget = timeToTarget;
}

SteeringOutput SteeringArrive::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    SteeringOutput steering;
    // Get the direction to target
    vec3 direction = target->position - character->position;
    direction.y =  0;
    f32 distance = length(direction);
    // Check if we are there, return no steering
    if(distance < targetRadius)
    {
        return {};
    }
    // If we are outside the slowRadius, the go max Speed
    f32 targetSpeed;
    if(distance > slowRadius)
    {
        targetSpeed = maxSpeed;
    }
    else
    {
        // Otherwise calculate a scale speed
        targetSpeed = maxSpeed * distance / slowRadius;
    }
    // Target velocity combines speed and direction
    vec3 targetVelocity = normalize(direction);
    targetVelocity *= targetSpeed;
    // Acceleration tries to get to the target velocity
    steering.linear = targetVelocity - character->velocity;
    steering.linear /= timeToTarget;
    // Chack if the acceleration is too fast
    if(length(steering.linear) > maxAcceleration)
    {
        steering.linear = normalize(steering.linear) * maxAcceleration;
    }
    steering.angular = 0.0f;
    return steering;
}

void SteeringAlign::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void SteeringAlign::SetTarget(Kinematic *target)
{
    this->target = target;
}

void SteeringAlign::SetMaxAngularAcceleration(f32 maxAngularAcceleration)
{
    this->maxAngularAcceleration = maxAngularAcceleration;
}

void SteeringAlign::SetMaxRotation(f32 maxRotation)
{
    this->maxRotation = maxRotation;
}

void SteeringAlign::SetTargetRadius(f32 targetRadius)
{
    this->targetRadius = targetRadius;
}

void SteeringAlign::SetSlowRadius(f32 slowRadius)
{
    this->slowRadius = slowRadius;
}

void SteeringAlign::SetTimeToTarget(f32 timeToTarget)
{
    this->timeToTarget = timeToTarget;
}

SteeringOutput SteeringAlign::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    SteeringOutput steering;

    f32 rotation = target->orientation - character->orientation;
    AdjustAngle(rotation);
    f32 rotationSize = fabsf(rotation);
    // Check if we are there, return no steering
    if(rotationSize < targetRadius)
    {
        return {};
    }
    // if we are outside the slowRadius, then use the maxRotation
    f32 targetRotation;
    if(rotationSize > slowRadius)
    {
        targetRotation = maxRotation;
    }
    else
    {
        targetRotation = maxRotation * rotationSize / slowRadius;
    }

    // the final target rotation combines speed and direction
    targetRotation *= rotation / rotationSize;
    steering.angular = targetRotation - character->rotation;
    steering.angular /= timeToTarget;

    // Check if the acceleration is to great
    f32 angularAcceleration = fabsf(steering.angular);
    if(angularAcceleration > maxAngularAcceleration)
    {
        steering.angular /= angularAcceleration;
        steering.angular *= maxAngularAcceleration;
    }

    steering.linear = vec3(0.0f);
    return steering;
}

void SteeringVelocityMach::SetCharacter(Kinematic *character)
{
    this->character = character;
}

void SteeringVelocityMach::SetTarget(Kinematic *target)
{
    this->target = target;
}

void SteeringVelocityMach::SetMaxAcceleration(f32 maxAcceleration)
{
    this->maxAcceleration = maxAcceleration;
}

void SteeringVelocityMach::SetTimeToTarget(f32 timeToTarget)
{
    this->timeToTarget = timeToTarget;
}

SteeringOutput SteeringVelocityMach::GetSteering()
{
    ASSERT(character != nullptr);
    ASSERT(target != nullptr);
    SteeringOutput steering;
    // Acceleration tries to get to the target velocity
    steering.linear = target->velocity - character->velocity;
    steering.linear /= timeToTarget;
    // Chack if the acceleration is too fast
    if(length(steering.linear) > maxAcceleration)
    {
        steering.linear = normalize(steering.linear) * maxAcceleration;
    }
    steering.angular = 0.0f;
    return steering;
}

void SteeringPursue::SetMaxPrediction(f32 maxPrediction)
{
    this->maxPrediction = maxPrediction;
}

SteeringOutput SteeringPursue::GetSteering()
{
    vec3 direction = target->position - character->position;
    direction.y =  0;
    f32 distance = length(direction);
    f32 speed = length(character->velocity);
    // Check if we are there, return no steering
    f32 prediction;
    if(speed <= (distance / maxPrediction))
    {
        prediction = maxPrediction;
    }
    else
    {
        prediction = distance / speed;
    }
    target->position += target->velocity * prediction;
    return SteeringArrive::GetSteering();
}

SteeringOutput SteeringFace::GetSteering()
{
    vec3 direction = target->position - character->position;
    direction.y =  0;

    if(length(direction) < 0.001f)
    {
        return {};
    }
    target->orientation = Kinematic::GetNewOrientation(target->orientation, direction);
    return SteeringAlign::GetSteering();
}

SteeringOutput SteeringLookWhereYoureGoing::GetSteering()
{
    if(length(character->velocity) < 0.001f)
    {
        return {};
    }
    target->orientation = Kinematic::GetNewOrientation(target->orientation, character->velocity);
    return SteeringAlign::GetSteering();
}

void SteeringWander::SetWanderOffset(f32 wanderOffset)
{
    this->wanderOffset = wanderOffset;
}

void SteeringWander::SetWanderRadius(f32 wanderRadius)
{
    this->wanderRadius = wanderRadius;
}

void SteeringWander::SetWanderRate(f32 wanderRate)
{
    this->wanderRate = wanderRate;
}

void SteeringWander::SetWanderOrientation(f32 wanderOrientation)
{
    this->wanderOrientation = wanderOrientation;
}

void SteeringWander::SetMaxAcceleration(f32 maxAcceleration)
{
    this->maxAcceleration = maxAcceleration;
}

SteeringOutput SteeringWander::GetSteering()
{
    wanderOrientation += RandomBinomial() * wanderRate;
    f32 targetOrientation = wanderOrientation + character->orientation;
    target->position = character->position + wanderOffset * normalize(vec3(-sinf(character->orientation), 0.0f, cosf(character->orientation)));
    target->position += wanderRadius * normalize(vec3(-sinf(targetOrientation), 0.0f, cosf(targetOrientation)));
    SteeringOutput steering = SteeringFace::GetSteering();
    steering.linear = maxAcceleration * normalize(vec3(-sinf(character->orientation), 0.0f, cosf(character->orientation)));
    return steering;
}