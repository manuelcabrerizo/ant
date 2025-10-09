#pragma once

#include "state.h"

#include <ui_renderer.h>
#include <text_renderer.h>
#include <actor_manager.h>
#include <notification_manager.h>

struct BloomUbo
{
    int horizontal;
    Vector3 pad;
};

class GameManager;
class Scene;
class PlayState : public IState, INotificable
{
private:
    Frame memoryFrame;

    ActorManager actorManager;
    GameManager* gameManager = nullptr;
    Scene* scene = nullptr;
    UIRenderer uiRenderer;
    UIRenderer frameRenderer;
    TextRenderer textRenderer;

    FrameBuffer* frameBuffer = nullptr;
    FrameBuffer* weaponFrameBuffer = nullptr;

    FrameBuffer* bloomBuffers[2] = { nullptr, nullptr };
    UniformBuffer* bloomUniformBuffer = nullptr;
    BloomUbo bloomUbo;
    float bloomScale = 1.0f / 2.0f;
    int bloomAmount = 2;

    int windowWidth;
    int windowHeight;

    float timeAccumulator;
    int frameCounter;
    int FPS;

    Vector2 crosshairSize;
    Vector2 crosshairPosition;

    bool isPlayerWin = false;
    bool isPlayerGameOver = false;
    int enemyCount = 0;
    int enemyKillCount = 0;
    int currentAmmo = 0;
    int maxAmmo = 0;
    int currentLife = 0;
    int maxLife = 0;

    void InitializeActorManager();
    void OnResize(OnResizeNotification* onResize);
    void OnPlayerWin(PlayerWinNotification* playerWin);
    void OnEnemySpawn(EnemySpawnNotification* enemySpawn);
    void OnEnemyKill(EnemyKillNotification* enemyKill);
    void OnAmmoChange(AmmoChangeNotification* ammoChange);
    void OnPlayerLifeChange(PlayerLifeChangeNotification* playerLifeChange);
public:
    void Init(GameManager *gameManager);
    void Terminate();

    void OnEnter() override;
    void OnExit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnNotify(NotificationType type, Notification* notification) override;

};