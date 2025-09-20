#include "fence_component.h"
#include "button_component.h"
#include "transform_component.h"

void FenceComponent::OnInit(ActorManager* actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::Signal);
    isListener = true;

    transform = owner->GetComponent<TransformComponent>();
    transform->position = startPosition;
    speed = 1.0f;
    distanceToMoveSq = (endPosition - startPosition).Magnitude();
}

void FenceComponent::OnTerminate(ActorManager* actorManager)
{
    if (isListener)
    {
        NotificationManager::Get()->RemoveListener(this, NotificationType::Signal);
    }
}

void FenceComponent::OnUpdate(ActorManager* actorManager, float deltaTime)
{
    if (!isActive) return;

    Vector3 toTarget = endPosition - transform->position;
    float distToTarget = toTarget.Magnitude();
    if (distToTarget > 0.001f)
    {
        Vector3 dir = toTarget.Normalized();
        float step = speed * deltaTime;
        if (step >= distToTarget)
        {
            transform->position = endPosition;
            isActive = false;
        }
        else
        {
            transform->position += dir * step;
        }
    }
}

void FenceComponent::OnObjectSignal(SignalNotification* onSignal)
{
    bool isValid = false;
    for (int i = 0; i < signables.size; i++)
    {
        if (onSignal->signable == signables[i])
        {
            isValid = true;
        }
    }
    if (!isValid) return;

    bool isSignal = true;
    for (int i = 0; i < signables.size; i++)
    {
        if (!signables[i]->IsSignal())
        {
            isSignal = false;
        }
    }

    if (isSignal)
    {
        isActive = true;
        isListener = false;
        NotificationManager::Get()->RemoveListener(this, NotificationType::Signal);
    }
}

void FenceComponent::SetStartPosition(const Vector3& position)
{
    this->startPosition = position;
}

void FenceComponent::SetEndPosition(const Vector3& position)
{
    this->endPosition = position;
}

void FenceComponent::AddButton(Actor* button)
{
    ISignable* signable = static_cast<ISignable*>(button->GetComponent<ButtonComponent>());
    signables.Push(signable);
}

void FenceComponent::OnNotify(NotificationType type, Notification* notification)
{
    switch (type)
    {
    case NotificationType::Signal: OnObjectSignal((SignalNotification*)notification); break;
    }
}