#pragma once

class EnemyComponent : public Component<EnemyComponent>, INotificable
{
private:
    TransformComponent *transform;
    SlotmapKey<INotificable *> playerMoveNotification;
    vec3 playerPosition;
public:
    void OnInit(ActorManager *actorManager);
    void OnTerminate(ActorManager *actorManager) override;
    void OnUpdate(ActorManager *actorManager, f32 dt);
    void OnNotify(NotificationType type, void *data, size_t size, void *sender) override;
};