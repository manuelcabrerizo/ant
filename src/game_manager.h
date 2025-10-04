#pragma once
#include <scene.h>
#include <states/state_machine.h>
#include <states/menu_state.h>
#include <states/play_state.h>
#include <graphics_manager.h>
#include <ui_renderer.h>

class GameManager
{
private:
    Array<Scene> scenes;

    FrameBuffer* frameBuffer = nullptr;
    UIRenderer uiRenderer;
    int clientWidth;
    int clientHeight;

    MenuState menuState;
    PlayState playState;
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

    Scene* GetCurrentScene();
};