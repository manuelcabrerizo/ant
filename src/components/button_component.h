#pragma once

#include "component.h"
#include <interfaces\ISignable.h>

class TransformComponent;

class ButtonComponent : public Component<ButtonComponent>, public ISignable
{
private:
    TransformComponent* transform = nullptr;
    Vector3 startPosition;
    Vector3 up;
    bool isSignal = false;
    bool wasSignal = false;
public:
    void OnInit(ActorManager* actorManager) override;
    bool IsSignal() override;
    void SetSignal(bool value);
};