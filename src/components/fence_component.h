#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>

class TransformComponent;

class FenceComponent : public Component<FenceComponent>
{
private:
    TransformComponent* transform;
    Vector3 startPosition;
    Vector3 endPosition;
public:
    void OnInit(ActorManager* actorManager);
    void OnUpdate(ActorManager* actorManager, f32 dt);

    void SetStartPosition(const Vector3& position);
    void SetEndPosition(const Vector3& position);

};