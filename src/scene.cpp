#include "scene.h"
#include "actor.h"

#include <asset_managers/model_manager.h>

#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/animation_component.h>

void Scene::Load(ActorManager* actorManager_, const char* filepath)
{
    this->actorManager = actorManager_;

    // Load the models
    ModelManager::Get()->Load("house", "data/models/House/source/house.fbx");
    ModelManager::Get()->Load("warrior", "data/models/Warrior/source/warrior.fbx");
    ModelManager::Get()->Load("test-level", "data/models/Level/source/level.fbx");
    ModelManager::Get()->Load("anim-gun", "data/models/fps-animations-vsk/source/FPS_VSK1.fbx");
    ModelManager::Get()->Load("tower", "data/models/MagicStudio/source/MagicStudio.fbx");
    ModelManager::Get()->Load("wizard", "data/models/Wizard/source/Wizard.FBX");

    // TODO: load all entities from the scene file
    // Create Entities
    actorManager->CreateActorFromFile("data/xml/player.xml");
    actorManager->CreateActorFromFile("data/xml/test-level.xml");
    actorManager->CreateActorFromFile("data/xml/tower.xml");
    actorManager->CreateActorFromFile("data/xml/house.xml");
    actorManager->CreateActorFromFile("data/xml/wizard.xml");
    SlotmapKey<Actor> enemy[3] =
    {
         actorManager->CreateActorFromFile("data/xml/enemy.xml"),
         actorManager->CreateActorFromFile("data/xml/enemy.xml"),
         actorManager->CreateActorFromFile("data/xml/enemy.xml")
    };

    // Warrior Animation
    TransformComponent* transforms[3] =
    {
         actorManager->GetComponent<TransformComponent>(enemy[0]),
         actorManager->GetComponent<TransformComponent>(enemy[1]),
         actorManager->GetComponent<TransformComponent>(enemy[2])
    };
    transforms[0]->position.x = 0.0f;
    transforms[1]->position.x = -2.0f;
    transforms[2]->position.x = -4.0f;
    RenderComponent* renders[3] =
    {
         actorManager->GetComponent<RenderComponent>(enemy[0]),
         actorManager->GetComponent<RenderComponent>(enemy[1]),
         actorManager->GetComponent<RenderComponent>(enemy[2])
    };
    renders[0]->isAnimated = true;
    renders[1]->isAnimated = true;
    renders[2]->isAnimated = true;

    AnimationComponent animation;
    animation.skeleton.Init("data/models/warrior.dae", STATIC_MEMORY);
    animation.animation.Init("data/animations/walk_front.dae", ModelManager::Get()->Get("warrior"), STATIC_MEMORY);
    actorManager->AddComponent<AnimationComponent>(enemy[0], animation);
    actorManager->AddComponent<AnimationComponent>(enemy[1], animation);
    actorManager->AddComponent<AnimationComponent>(enemy[2], animation);
}

void Scene::Unload()
{
    ASSERT(actorManager != nullptr);
    actorManager->Clear();

    // Load the models
    ModelManager::Get()->Unload("house");
    ModelManager::Get()->Unload("warrior");
    ModelManager::Get()->Unload("test-level");
    ModelManager::Get()->Unload("anim-gun");
    ModelManager::Get()->Unload("tower");
    ModelManager::Get()->Unload("wizard");
}