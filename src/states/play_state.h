#pragma once

#include "state.h"

#include <ui_renderer.h>
#include <actor_manager.h>
#include <notification_manager.h>

class GameManager;
class Scene;
class BatchRenderer;

class PlayState : public IState, INotificable
{
private:
    Frame memoryFrame;

    ActorManager actorManager;
    GameManager* gameManager = nullptr;
    Scene* scene = nullptr;
    UIRenderer uiRenderer;

    int windowWidth;
    int windowHeight;

    Vector2 crosshairSize;
    Vector2 crosshairPosition;

    BatchRenderer *batchRenderer = nullptr;

    void InitializeActorManager();
    void OnResize(OnResizeNotification* onResize);
public:
    void Init(GameManager *gameManager);
    void Terminate();

    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnNotify(NotificationType type, Notification* notification) override;

};