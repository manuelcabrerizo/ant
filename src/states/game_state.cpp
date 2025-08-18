#include "game_state.h"
#include <actor_manager.h>
#include <scene.h>

#include <components/player_controller_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/enemy_component.h>
#include <components/physics_component.h>
#include <components/render_component.h>
#include <components/bullet_component.h>

void GameState::Init(ActorManager* actorManager, Scene* scene)
{
    this->actorManager = actorManager;
    this->scene = scene;
}

void GameState::OnEnter()
{
    scene->Load(actorManager, "");
}

void GameState::OnExit()
{
    scene->Unload();
}

void GameState::OnUpdate(float deltaTime)
{
    // Initialize new components
    actorManager->InitializeNewComponents();
    // Update
    actorManager->UpdateComponents<PlayerControllerComponent>(deltaTime);
    actorManager->UpdateComponents<CameraComponent>(deltaTime);
    actorManager->UpdateComponents<WeaponComponent>(deltaTime);
    actorManager->UpdateComponents<EnemyComponent>(deltaTime);
    actorManager->UpdateComponents<PhysicsComponent>(deltaTime);
    actorManager->UpdateComponents<RenderComponent>(deltaTime);
    actorManager->UpdateComponents<BulletComponent>(deltaTime);

    actorManager->ProcessActorsToRemove();
}

void GameState::OnRender()
{
    actorManager->RenderComponents<RenderComponent>();
}
