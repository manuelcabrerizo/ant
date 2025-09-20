#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>

class PortalComponent : public Component<PortalComponent>
{
private:
    Vector3 dstPos;
    float dstRot;
public:
    void SetDestination(const Vector3& dstPos, float dstRot);
    Vector3 GetDestPosition();
    float GetDestRotation();
};