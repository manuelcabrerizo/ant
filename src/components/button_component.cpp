#include "button_component.h"
#include "transform_component.h"
#include <audio_manager.h>

void ButtonComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();
    startPosition = transform->position;
    up = Matrix4::TransformVector(Matrix4::TransformFromEuler(transform->rotation), Vector3::up);
}

bool ButtonComponent::IsSignal()
{
    return isSignal;
}

void ButtonComponent::SetSignal(bool value)
{
    wasSignal = isSignal;
    isSignal = value;

    if (isSignal && !wasSignal)
    {
        transform->position -= up * 0.1f;
        AudioManager::Get()->PlaySoundFx(SoundName::ButtonPush, false);
    }
}
