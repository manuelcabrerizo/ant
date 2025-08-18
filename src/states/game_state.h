#pragma once

#include "state.h"

class ActorManager;
class Scene;

class GameState : public IState
{
private:
    ActorManager* actorManager;
    Scene* scene;
public:
    void Init(ActorManager* actorManager, Scene* scene);

    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
};