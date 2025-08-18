#include "menu_state.h"
#include <game_manager.h>
#include <input_manager.h>

void MenuState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
}

void MenuState::OnEnter()
{
}

void MenuState::OnExit()
{
}

void MenuState::OnUpdate(float deltaTime)
{
    if (InputManager::Get()->KeyJustDown(KEY_RETURN))
    {
        gameManager->ChangeToGameState();
    }
}

void MenuState::OnRender()
{
}