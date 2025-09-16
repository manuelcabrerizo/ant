#include "fence_component.h"
#include "button_component.h"
#include "transform_component.h"

void FenceComponent::OnInit(ActorManager* actorManager)
{
    NotificationManager::Get()->AddListener(this, NotificationType::Signal);

    transform = owner->GetComponent<TransformComponent>();
    speed = 0.5f;
}

void FenceComponent::OnTerminate(ActorManager* actorManager)
{
    NotificationManager::Get()->RemoveListener(this, NotificationType::Signal);
}

void FenceComponent::OnUpdate(ActorManager* actorManager, float deltaTime)
{
    if (!isActive  || timer > 1.0f) return;

    if (timer < 1.0f)
    {
        Vector3 position = startPosition.Lerp(endPosition, timer);
        transform->position = position;
    }
    else
    {
        transform->position = endPosition;
    }
    timer += speed * deltaTime;
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