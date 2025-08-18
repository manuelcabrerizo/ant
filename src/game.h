#pragma once

#include <actor_manager.h>
#include <states/state_machine.h>
#include <scene.h>

#include <states/menu_state.h>
#include <states/game_state.h>

class Game
{
private:
    Array<Scene> scenes;
    ActorManager actorManager;

    MenuState menuState;
    GameState gameState;
    StateMachine stateMachine;

    void InitializeAssetsManagers();
    void ShutdownAssetsManagers();

    void InitializeActorManager();
    void LoadDefaultAssets();
public:     
    void Init();
    void Terminate();
    void Update(f32 dt);
    void Render(f32 dt);
};
