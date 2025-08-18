#include "play_state.h"
#include <game_manager.h>
#include <input_manager.h>
#include <components/player_controller_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/enemy_component.h>
#include <components/physics_component.h>
#include <components/collider_component.h>
#include <components/render_component.h>
#include <components/bullet_component.h>

void PlayState::Init(GameManager *gameManager)
{
    this->gameManager = gameManager;
    this->actorManager = gameManager->GetActorManager();
    this->scene = gameManager->GetCurrentScene();
}

void PlayState::OnEnter()
{
    scene->Load(actorManager, "");
}

void PlayState::OnExit()
{
    scene->Unload();
}

void PlayState::OnUpdate(float deltaTime)
{
    // Initialize new components
    actorManager->InitializeNewComponents();
    // Update
    actorManager->UpdateComponents<PlayerControllerComponent>(deltaTime);
    actorManager->UpdateComponents<CameraComponent>(deltaTime);
    actorManager->UpdateComponents<WeaponComponent>(deltaTime);
    actorManager->UpdateComponents<EnemyComponent>(deltaTime);
    actorManager->UpdateComponents<PhysicsComponent>(deltaTime);
    actorManager->UpdateComponents<ColliderComponent>(deltaTime);
    actorManager->UpdateComponents<RenderComponent>(deltaTime);
    actorManager->UpdateComponents<BulletComponent>(deltaTime);
    // Late Update
    actorManager->LateUpdateComponents<PhysicsComponent>(deltaTime);

    actorManager->ProcessActorsToRemove();

    if (InputManager::Get()->KeyJustDown(KEY_P))
    {
        gameManager->ChangeToMenuState();
    }
}

void PlayState::OnRender()
{
    actorManager->RenderComponents<RenderComponent>();
}