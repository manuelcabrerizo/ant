#include "scene.h"
#include "actor.h"

#include <asset_managers/model_manager.h>
#include <asset_managers/animation_manager.h>

#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/animation_component.h>

#include <strings.h>

void Scene::Load(ActorManager* actorManager_, const char* filepath)
{
    this->actorManager = actorManager_;

    // Load the models
    ModelManager::Get()->Load("enemy", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("anim-gun", "data/models/Revolver/source/Retribution.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("bullet", "data/models/testBullet.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("level1", "data/models/TestLevel/source/TestLevel.fbx", FRAME_MEMORY);

    // Load animation data
    SkeletonManager::Get()->Load("Bloodwraith", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    AnimationManager::Get()->Load("Walking", "data/animations/Mutant Walking.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Death", "data/animations/Zombie Death.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);

    // Create the level
    actorManager->CreateActorFromFile("data/xml/level1.xml");
    
#if 1
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

    File file = PlatformReadFile("data/entities/enemies1.txt", SCRATCH_MEMORY);
    char* text = (char*)file.data;
    int endOfFilePos = strlen(text);
    int currentPos = 0;

    char name[256];
    Vector3 position;
    while (currentPos < endOfFilePos)
    {
        char* line = text + currentPos;
        if (sscanf(line, "%s position: [%f, %f, %f]", name, &position.x, &position.y, &position.z) == 4)
        {
            // Create the enemy and position it
            Actor* enemy = actorManager->CreateActorFromFile("data/xml/enemy.xml");
            TransformComponent* transform = enemy->GetComponent<TransformComponent>();
            transform->position = position;

            // Set up the enemy animation component
            // TODO: load this component from a file
            actorManager->AddComponent<AnimationComponent>(enemy, {});
            RenderComponent* render = enemy->GetComponent<RenderComponent>();
            render->isAnimated = true;

            break;
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }

        int distance = Strings::FindFirstInstance(line, '\n');
        if (distance == -1)
        {
            break;
        }
        currentPos += distance + 1;
    }
    
    MemoryManager::Get()->ReleaseFrame(frame);
#endif
    // Create the player
    actorManager->CreateActorFromFile("data/xml/player.xml");
}

void Scene::Unload()
{
    ASSERT(actorManager != nullptr);
    actorManager->Clear();

    // Unload Animations
    SkeletonManager::Get()->Unload("Bloodwraith");
    AnimationManager::Get()->Unload("Death");
    AnimationManager::Get()->Unload("Walking");

    // Unload the models
    ModelManager::Get()->Unload("enemy");
    ModelManager::Get()->Unload("anim-gun");
    ModelManager::Get()->Unload("bullet");
    ModelManager::Get()->Unload("level1");
}