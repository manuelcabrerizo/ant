#pragma once
#include <scene.h>
#include <actor_manager.h>
#include <states/state_machine.h>
#include <states/menu_state.h>
#include <states/play_state.h>

class GameManager
{
private:
    Array<Scene> scenes;
    ActorManager actorManager;

    MenuState menuState;
    PlayState gameState;
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

    void ChangeToMenuState();
    void ChangeToGameState();

    ActorManager* GetActorManager();
    Scene* GetCurrentScene();
};
