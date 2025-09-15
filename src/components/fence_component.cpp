#include "fence_component.h"

void FenceComponent::OnInit(ActorManager* actorManager)
{
}

void FenceComponent::SetStartPosition(const Vector3& position)
{
    this->startPosition = position;
}

void FenceComponent::SetEndPosition(const Vector3& position)
{
    this->endPosition = position;
}