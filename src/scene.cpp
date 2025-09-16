#include "scene.h"
#include "actor.h"

#include <asset_managers/model_manager.h>
#include <asset_managers/animation_manager.h>

#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/animation_component.h>
#include <components/collider_component.h>
#include <components/portal_component.h>
#include <components/effect_component.h>
#include <components/key_component.h>
#include <components/fence_component.h>
#include <components/button_component.h>

#include <strings.h>
#include <math/algebra.h>

void Scene::Load(ActorManager* actorManager_, const char* filepath)
{
    this->actorManager = actorManager_;

    // Load the models
    ModelManager::Get()->Load("enemy", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("anim-gun", "data/models/Revolver/source/Retribution.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("bullet", "data/models/testBullet.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("level1", "data/models/TestLevel/source/TestLevel.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("portal", "data/models/Portal/source/Portal.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("key", "data/models/Key/source/Key.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("fence", "data/models/Fence/source/Fence.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("button", "data/models/Button/source/button.fbx", FRAME_MEMORY);

    // Load animation data
    SkeletonManager::Get()->Load("Bloodwraith", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    AnimationManager::Get()->Load("Walking", "data/animations/Mutant Walking.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Death", "data/animations/Zombie Death.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);

    // Create the level
    actorManager->CreateActorFromFile("data/xml/level1.xml");

    // Create the key
    Actor* key = actorManager->CreateActorFromFile("data/xml/key.xml");
    KeyComponent keyComponent;
    actorManager->AddComponent<KeyComponent>(key, keyComponent);
    TransformComponent* keyTransform = key->GetComponent<TransformComponent>();
    keyTransform->position = Vector3(59.562684376750326f, -29, 32.048033260074035f);

    // Create the fence
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File fencesFile = PlatformReadFile("data/entities/fences.txt", SCRATCH_MEMORY);
    FileReader reader = FileReader(&fencesFile);
    while (const char* line = reader.GetNextLine())
    {
        Actor* fence = actorManager->CreateActorFromFile("data/xml/fence.xml");
        TransformComponent* pTransform = fence->GetComponent<TransformComponent>();
        Vector3 sPos,ePos, rot, sca;
        Vector3 bPos, bSca, bRot;
        if (sscanf(line, "p: [[%f, %f, %f], [%f, %f, %f]] rs: [[%f, %f, %f], [%f, %f, %f]] b: [[%f, %f, %f], [%f, %f, %f], [%f, %f, %f]]",
            &sPos.x, &sPos.y, &sPos.z, &ePos.x, &ePos.y, &ePos.z, &rot.x, &rot.y, &rot.z, &sca.x, &sca.y, &sca.z,
            &bPos.x, &bPos.y, &bPos.z, &bRot.x, &bRot.y, &bRot.z, &bSca.x, &bSca.y, &bSca.z) == 21)
        {
            rot *= 1.0f / 180.0f;
            rot *= (float)ANT_PI;
            bRot *= 1.0f / 180.0f;
            bRot *= (float)ANT_PI;

            pTransform->position = sPos;
            pTransform->rotation = rot;
            pTransform->scale = sca;

            // Add collider
            ColliderComponent colliderComponent;
            colliderComponent.Init(3, FRAME_MEMORY);

            Matrix4 rotMat = Matrix4::TransformFromEuler(pTransform->rotation);
            Vector3 front = Matrix4::TransformVector(rotMat, Vector3::forward);
            Vector3 right = Matrix4::TransformVector(rotMat, Vector3::right);
            Vector3 up = Matrix4::TransformVector(rotMat, Vector3::up);

            Vector3 rotation[] = { right, up, front };

            OBB obb;
            obb.Init(Vector3::zero, rotation, Vector3(pTransform->scale.x*0.5f, pTransform->scale.y*0.5f, 0.25f));
            Collider collider_ = Collider(obb, fence);
            colliderComponent.AddSubCollider(collider_);
            actorManager->AddComponent(fence, colliderComponent);

            // Create the button
            Actor* button = actorManager->CreateActorFromFile("data/xml/button.xml");
            TransformComponent* bTransform = button->GetComponent<TransformComponent>();
            ColliderComponent* bCollider = button->GetComponent<ColliderComponent>();
            bTransform->position = bPos;
            bTransform->rotation = bRot;
            bTransform->scale = bSca;
            bCollider->SetIsTrigger(true);
            ButtonComponent buttonComponent;
            actorManager->AddComponent(button, buttonComponent);

            // Add Fence Component
            FenceComponent fenceComponent;
            fenceComponent.SetStartPosition(sPos);
            fenceComponent.SetEndPosition(ePos);
            fenceComponent.AddButton(button);
            actorManager->AddComponent(fence, fenceComponent);
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);


    // Temp fix, EffectComponent is to big to be created on the stack
    // TODO: use the scratch memory for this
    {
        Actor* box = actorManager->CreateActorFromFile("data/xml/default.xml");
        TransformComponent* boxTrans = box->GetComponent<TransformComponent>();
        boxTrans->position = Vector3(0, -2.5f, 9.5f);
        EffectComponent* effect = new EffectComponent();
        effect->SetSpawnArea(Vector3(-4, 0, -2), Vector3(4, 0, 2));
        actorManager->AddComponent<EffectComponent>(box, *effect);
        delete effect;
    }
    {
        Actor* box = actorManager->CreateActorFromFile("data/xml/default.xml");
        TransformComponent* boxTrans = box->GetComponent<TransformComponent>();
        boxTrans->position = Vector3(22.5f, -2, 34.75f);
        EffectComponent* effect = new EffectComponent();
        effect->SetSpawnArea(Vector3(-2.5, 0, -2.5), Vector3(2.5, 0, 2.5));
        actorManager->AddComponent<EffectComponent>(box, *effect);
        delete effect;
    }

    // Create the portals
    frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File portalsFile = PlatformReadFile("data/entities/portals.txt", SCRATCH_MEMORY);
    reader = FileReader(&portalsFile);
    while (const char* line = reader.GetNextLine())
    {
        Actor* portal = actorManager->CreateActorFromFile("data/xml/portal.xml");
        TransformComponent* pTransform = portal->GetComponent<TransformComponent>();
        PortalComponent* portalCmp = portal->GetComponent<PortalComponent>();
        Vector3 pPos, pDst;
        float rPos, rDst;
        if (sscanf(line, "portal: [[%f, %f, %f], %f] dst: [[%f, %f, %f], %f]",
            &pPos.x, &pPos.y, &pPos.z, &rPos, &pDst.x, &pDst.y, &pDst.z, &rDst) == 8)
        {
            rDst += 180.0;
            pTransform->position = pPos;
            pTransform->rotation.y = rPos/180.0f*ANT_PI;
            portalCmp->SetDestination(pDst, (rDst/180.0f) * ANT_PI);
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
    
    // Create the Enemies
    frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile("data/entities/enemies1.txt", SCRATCH_MEMORY);
    reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
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
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);


    // Create the player
    actorManager->CreateActorFromFile("data/xml/player.xml");
}

void Scene::Unload()
{
}