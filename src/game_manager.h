#pragma once
#include <scene.h>
#include <states/state_machine.h>
#include <states/menu_state.h>
#include <states/play_state.h>
#include <states/pause_state.h>
#include <states/victory_state.h>
#include <states/game_over_state.h>

class GameManager
{
private:
    Array<Scene> scenes;
    int clientWidth;
    int clientHeight;

    MenuState menuState;
    PlayState playState;
    PauseState pauseState;
    VictoryState victoryState;
    GameOverState gameOverState;

    StateMachine stateMachine;

    void InitializeAssetsManagers();
    void ShutdownAssetsManagers();

    void LoadDefaultAssets();
public:     
    void Init();
    void Terminate();
    void Update(f32 dt);
    void Render(f32 dt);

    void ChangeToMenuState();
    void ChangeToPlayState();
    void ChangeToVictoryState();
    void ChangeToGameOverState();
    void PushPauseState();
    void PopPauseState();

    Scene* GetCurrentScene();
};