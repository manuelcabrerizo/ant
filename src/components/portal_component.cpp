#include "portal_component.h"

void PortalComponent::SetDestination(const Vector3& dstPos, float dstRot)
{
    this->dstPos = dstPos;
    this->dstRot = dstRot;
}

Vector3 PortalComponent::GetDestPosition()
{
    return dstPos;
}

float PortalComponent::GetDestRotation()
{
    return dstRot;
}
