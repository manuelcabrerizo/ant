#pragma once

#include "state.h"
#include <actor_manager.h>

class GameManager;
class Scene;

class PlayState : public IState
{
private:
    Frame memoryFrame;

    ActorManager actorManager;
    GameManager* gameManager = nullptr;
    Scene* scene = nullptr;

    void InitializeActorManager();
public:
    void Init(GameManager *gameManager);
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
};