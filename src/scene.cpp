#include "scene.h"
#include "actor.h"

#include <asset_managers/model_manager.h>
#include <asset_managers/animation_manager.h>

#include <components/transform_component.h>
#include <components/camera_component.h>
#include <components/render_component.h>
#include <components/animation_component.h>
#include <components/collider_component.h>
#include <components/portal_component.h>
#include <components/effect_component.h>
#include <components/fence_component.h>
#include <components/button_component.h>

#include <fstream>
#include <strings.h>
#include <math/algebra.h>
#include <components/tiled_render_component.h>
#include <collisions/collider.h>

void Scene::Load(ActorManager* actorManager_, const char* filepath)
{
    this->actorManager = actorManager_;

    LoadAssets();
    actorManager->CreateActorFromFile("data/xml/level1.xml");
    CreateFenceActors("data/xml/fence.xml", "data/entities/fences.txt");
    CreateEffectActors();
    CreatePortalActors("data/xml/portal.xml", "data/entities/portals.txt");
    CreateEnemyActors("data/xml/enemy.xml", "data/entities/enemies1.txt");
    CreateEndTriggerActors("data/entities/endTriggers.txt");
    CreateDeadTriggerActors("data/entities/deadTriggers.txt");
    CreateAmmoActors("data/xml/ammo.xml", "data/entities/ammos.txt");
    CreateHealActors("data/xml/heal.xml", "data/entities/heals.txt");
    Actor* player = actorManager->CreateActorFromFile("data/xml/player.xml");
    actorManager->InitializeNewComponents();
    CreateLights("data/entities/Lights.txt");
}

void Scene::Unload()
{
}

void Scene::LoadAssets()
{
    // Load the models
    ModelManager::Get()->Load("enemy", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("anim-gun", "data/models/Revolver/source/Retribution.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("bullet", "data/models/testBullet.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("level1", "data/models/TestLevel/source/TestLevel.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("portal", "data/models/Portal/source/Portal.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("ammo", "data/models/Ammo/source/Ammo.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("heal", "data/models/Heal/Heal.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("fence", "data/models/Fence/source/Fence.fbx", FRAME_MEMORY);
    ModelManager::Get()->Load("button", "data/models/Button/source/button.fbx", FRAME_MEMORY);

    // Load animation data
    SkeletonManager::Get()->Load("Bloodwraith", "data/models/bloodwraith/source/bloodwraith.fbx", FRAME_MEMORY);
    AnimationManager::Get()->Load("Walking", "data/animations/Mutant Walking.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Death", "data/animations/Zombie Death.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Idle", "data/animations/Orc Idle.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Hit", "data/animations/Zombie Reaction Hit.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
    AnimationManager::Get()->Load("Attack", "data/animations/Zombie Attack.fbx", ModelManager::Get()->Get("enemy"), FRAME_MEMORY);
}

void Scene::CreateFenceActors(const char* fenceActorFile, const char* fenceInstancesFile)
{
    // Create the fence
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File fencesFile = PlatformReadFile(fenceInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&fencesFile);
    while (const char* line = reader.GetNextLine())
    {
        Actor* fence = actorManager->CreateActorFromFile(fenceActorFile);
        TransformComponent* pTransform = fence->GetComponent<TransformComponent>();
        Vector3 sPos, ePos, rot, sca;
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
            obb.Init(Vector3::zero, rotation, Vector3(pTransform->scale.x * 0.5f, pTransform->scale.y * 0.5f, 0.25f));
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
}

void Scene::CreateEffectActors()
{
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
}

void Scene::CreatePortalActors(const char* portalActorFile, const char* portalInstancesFile)
{
    // Create the portals
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File portalsFile = PlatformReadFile(portalInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&portalsFile);
    while (const char* line = reader.GetNextLine())
    {
        Actor* portalVisual = actorManager->CreateActorFromFile(portalActorFile);
        TransformComponent* pTransform = portalVisual->GetComponent<TransformComponent>();
        Vector3 pPos, pDst;
        float rPos, rDst;
        if (sscanf(line, "portal: [[%f, %f, %f], %f] dst: [[%f, %f, %f], %f]",
            &pPos.x, &pPos.y, &pPos.z, &rPos, &pDst.x, &pDst.y, &pDst.z, &rDst) == 8)
        {
            rDst += 180.0;
            pTransform->position = pPos;
            pTransform->rotation.y = rPos / 180.0f * ANT_PI;

            // TODO: optimice the collider be initialize using euler angles
            Matrix4 rotMat = Matrix4::TransformFromEuler(pTransform->rotation);
            Vector3 front = Matrix4::TransformVector(rotMat, Vector3::forward);
            Vector3 right = Matrix4::TransformVector(rotMat, Vector3::right);
            Vector3 up = Matrix4::TransformVector(rotMat, Vector3::up);

            Vector3 rotation[] = { right, up, front };

            // Add Collider components
            {
                ColliderComponent colliderComponent;
                colliderComponent.Init(3, FRAME_MEMORY);
                colliderComponent.SetOffset(Vector3(0, 1, 0));

                // Left Collider
                {
                    OBB obb;
                    obb.Init(Vector3::zero, rotation, Vector3(0.5, 2, 0.5));
                    Collider collider_ = Collider(obb, portalVisual);
                    collider_.SetOffset(up * 1.f + right * -1.5);
                    colliderComponent.AddSubCollider(collider_);
                }

                // Right Collider
                {
                    OBB obb;
                    obb.Init(Vector3::zero, rotation, Vector3(0.5, 2, 0.5));
                    Collider collider_ = Collider(obb, portalVisual);
                    collider_.SetOffset(up * 1.f + right * 1.5);
                    colliderComponent.AddSubCollider(collider_);
                }

                // Up Collider
                {
                    OBB obb;
                    obb.Init(Vector3::zero, rotation, Vector3(2, 0.5, 0.5));
                    Collider collider_ = Collider(obb, portalVisual);
                    collider_.SetOffset(up * 3.f);
                    colliderComponent.AddSubCollider(collider_);
                }

                actorManager->AddComponent(portalVisual, colliderComponent);
            }

            // Create the trigger actor
            {
                Actor* portalTrigger = actorManager->CreateActor();
                portalTrigger->SetTag(ActorTag::Portal);
                TransformComponent transformCmp = *pTransform;
                actorManager->AddComponent(portalTrigger, transformCmp);

                PortalComponent portalCmp;
                portalCmp.SetDestination(pDst, (rDst / 180.0f) * ANT_PI);
                actorManager->AddComponent(portalTrigger, portalCmp);

                {
                    Vector3 offset = Vector3(0, 0.5f, 0);
                    Vector3 position = transformCmp.position + offset;
                    OBB obb;
                    obb.Init(transformCmp.position, rotation, Vector3(1.5, 2, 0.25));
                    Collider collider_ = Collider(obb, portalTrigger);
                    collider_.SetOffset(up * 1.5f + front * 0.3);

                    ColliderComponent colliderComponent;
                    colliderComponent.Init(1, FRAME_MEMORY);
                    colliderComponent.AddSubCollider(collider_);
                    colliderComponent.SetIsTrigger(true);
                    actorManager->AddComponent(portalTrigger, colliderComponent);
                }
            }
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateEnemyActors(const char* enemyActorFile, const char* enemyInstancesFile)
{
    // Create the Enemies
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(enemyInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
        if (sscanf(line, "%s position: [%f, %f, %f]", name, &position.x, &position.y, &position.z) == 4)
        {
            // Create the enemy and position it
            Actor* enemy = actorManager->CreateActorFromFile(enemyActorFile);
            TransformComponent* transform = enemy->GetComponent<TransformComponent>();
            transform->position = position;

            // Set up the enemy animation component
            actorManager->AddComponent<AnimationComponent>(enemy, {});
            RenderComponent* render = enemy->GetComponent<RenderComponent>();
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateEndTriggerActors(const char* endTriggerInstancesFile)
{
    // Create the End Trigger
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(endTriggerInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
        Vector3 scale;
        if (sscanf(line, "[[%f, %f, %f], [%f, %f, %f]]", &position.x, &position.y, &position.z, &scale.x, &scale.y, &scale.z) == 6)
        {
            // Create the enemy and position it
            Actor* endTrigger = actorManager->CreateActor();
            endTrigger->SetTag(ActorTag::EndTrigger);

            // Add transform component
            TransformComponent transform;
            transform.position = position;
            transform.scale = Vector3(1, 1, 1);
            transform.rotation = Vector3(0, 0, 0);
            actorManager->AddComponent<TransformComponent>(endTrigger, transform);

            // Add collision component
            Vector3 hScale = scale * 0.5f;
            Vector3 min = position - Vector3::right * hScale.x - Vector3::up * hScale.y - Vector3::forward * hScale.z;
            Vector3 max = position + Vector3::right * hScale.x + Vector3::up * hScale.y + Vector3::forward * hScale.z;

            AABB aabb;
            aabb.Init(min, max);
            Collider collider(aabb, endTrigger);

            ColliderComponent colliderComponent;
            colliderComponent.Init(1, FRAME_MEMORY);
            colliderComponent.AddSubCollider(collider);
            colliderComponent.SetIsTrigger(true);

            actorManager->AddComponent<ColliderComponent>(endTrigger, colliderComponent);
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateDeadTriggerActors(const char* deadTriggerInstancesFile)
{
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(deadTriggerInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
        Vector3 scale;
        if (sscanf(line, "[[%f, %f, %f], [%f, %f, %f]]", &position.x, &position.y, &position.z, &scale.x, &scale.y, &scale.z) == 6)
        {
            // Create the enemy and position it
            Actor* endTrigger = actorManager->CreateActor();
            endTrigger->SetTag(ActorTag::DeadTrigger);

            // Add transform component
            TransformComponent transform;
            transform.position = position;
            transform.scale = Vector3(1, 1, 1);
            transform.rotation = Vector3(0, 0, 0);
            actorManager->AddComponent<TransformComponent>(endTrigger, transform);

            // Add collision component
            Vector3 hScale = scale * 0.5f;
            Vector3 min = position - Vector3::right * hScale.x - Vector3::up * hScale.y - Vector3::forward * hScale.z;
            Vector3 max = position + Vector3::right * hScale.x + Vector3::up * hScale.y + Vector3::forward * hScale.z;

            AABB aabb;
            aabb.Init(min, max);
            Collider collider(aabb, endTrigger);

            ColliderComponent colliderComponent;
            colliderComponent.Init(1, FRAME_MEMORY);
            colliderComponent.AddSubCollider(collider);
            colliderComponent.SetIsTrigger(true);

            actorManager->AddComponent<ColliderComponent>(endTrigger, colliderComponent);
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateAmmoActors(const char* ammoActorFile, const char* ammoInstancesFile)
{
    // Create the Ammos
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(ammoInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
        Vector3 scale;
        if (sscanf(line, "[[%f, %f, %f], [%f, %f, %f]]", &position.x, &position.y, &position.z, &scale.x, &scale.y, &scale.z) == 6)
        {
            Actor* ammo = actorManager->CreateActorFromFile(ammoActorFile);
            TransformComponent* transform = ammo->GetComponent<TransformComponent>();
            transform->position = position;
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateHealActors(const char* healActorFile, const char* healInstancesFile)
{
    // Create the Heals
    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(healInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        char name[256];
        Vector3 position;
        Vector3 scale;
        if (sscanf(line, "[[%f, %f, %f], [%f, %f, %f]]", &position.x, &position.y, &position.z, &scale.x, &scale.y, &scale.z) == 6)
        {
            Actor* ammo = actorManager->CreateActorFromFile(healActorFile);
            TransformComponent* transform = ammo->GetComponent<TransformComponent>();
            transform->position = position;
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}

void Scene::CreateLights(const char* lightsInstancesFile)
{
    // Add Lights
    DirectionalLight light;
    light.ambient = Vector4(0.1, 0.1, 0.05, 0);
    light.diffuse = Vector4(2.125, 1, 1.5, 0);
    light.specular = Vector4(1.5, 1.5, 1.5, 0);
    light.direction = Vector3(2, -1, 1);
    light.direction.Normalize();
    GraphicsManager::Get()->SetDirectionalLight(light);

    // Create the Points Lights
    PointLight pointLights[8];
    int lightsCount = 0;

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    File file = PlatformReadFile(lightsInstancesFile, SCRATCH_MEMORY);
    FileReader reader = FileReader(&file);
    while (const char* line = reader.GetNextLine())
    {
        ASSERT(lightsCount < 8);
        char name[256];
        Vector3 position;
        Vector3 scale;
        if (sscanf(line, "[[%f, %f, %f], [%f, %f, %f]]", &position.x, &position.y, &position.z, &scale.x, &scale.y, &scale.z) == 6)
        {
            PointLight pointLight;
            pointLight.ambient = Vector4(0.2, 0.1, 0.3, 0);
            pointLight.diffuse = Vector4(1.2, 2.5, 1.25, 1);
            pointLight.specular = Vector4(0.1, 0.0, 0.3, 1);
            pointLight.position = position;
            pointLight.range = scale.x * 2.5f;
            pointLight.att = Vector3();
            pointLights[lightsCount++] = pointLight;
        }
        else
        {
            ASSERT(!"ERROR: invalid file format");
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);

    GraphicsManager::Get()->AddPointLights(pointLights, lightsCount);
}