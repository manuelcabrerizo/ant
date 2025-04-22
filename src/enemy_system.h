#pragma once

class EnemySystem : public INotificable
{
private:
    // Blackboard 
    vec3 playerPosition;

    void OnNotify(NotificationType type, Notification notification, void *sender) override;
public:
    void Init();
    void Update(ActorManager *actorManager, f32 dt);
    void Terminate();
};