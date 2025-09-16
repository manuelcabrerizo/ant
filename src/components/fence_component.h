#pragma once

#include "component.h"
#include <math/vector3.h>
#include <collisions/collider.h>
#include <interfaces/ISignable.h>
#include <notification_manager.h>


class TransformComponent;

class FenceComponent : public Component<FenceComponent>, public INotificable
{
private:
    TransformComponent* transform;
    Vector3 startPosition;
    Vector3 endPosition;

    float speed = 1.0f;
    float distanceToMoveSq;

    bool isActive = false;
    bool isListener = true;

    StaticArray<ISignable*, 4> signables;

    void OnObjectSignal(SignalNotification* onSignal);
public:
    void OnInit(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, float deltaTime);
    void OnTerminate(ActorManager* actorManager) override;
    void OnNotify(NotificationType type, Notification* notification) override;

    void SetStartPosition(const Vector3& position);
    void SetEndPosition(const Vector3& position);
    void AddButton(Actor* button);
};