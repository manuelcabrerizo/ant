#pragma once

class EnemySystem : public INotificable
{
private:
    // Blackboard 
    vec3 playerPosition;

    void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
public:
    void Init();
    void Update(ActorManager *actorManager, f32 dt);
    void Terminate();
};