#include "actor_manager.h"

#include <components/component.h>
#include <components/transform_component.h>
#include <components/render_component.h>
#include <components/weapon_render_component.h>
#include <components/animated_render_component.h>
#include <components/tiled_render_component.h>
#include <components/physics_component.h>
#include <components/collider_component.h>
#include <components/camera_component.h>
#include <components/weapon_component.h>
#include <components/anchor_component.h>
#include <components/player_controller_component.h>
#include <components/enemy_component.h>
#include <components/portal_component.h>

#include <math/algebra.h>
#include <math/vector3.h>

#include <tinyxml2.h>
#include <asset_managers/model_manager.h>
#include <asset_managers/material_manager.h>
#include <utils.h>

#include <collisions/collider.h>

int ActorManager::actorGeneration = 1;

void ActorManager::BeingInitialization(i32 actorCount, i32 componentTypeCount_, i32 memoryType_)
{
    actorGeneration = 1;
    memoryType = memoryType_;
    componentTypeCount = componentTypeCount_;
    maxActorCount = actorCount;
    maxComponentCount = 0;

    actors.Init(maxActorCount, memoryType);
    actorsComponentsMap.Init(maxActorCount, memoryType);
    componentListMap.Init(componentTypeCount, memoryType);
    componentListToRemove.Init(componentTypeCount, memoryType);
}

void ActorManager::EndInitialization()
{
    componentsToInit.Init(maxComponentCount, memoryType);
}

void ActorManager::Terminate()
{
    for (int i = 0; i < componentListToRemove.size; i++)
    {
        componentListToRemove[i]->Clear(this);
    }
    actors.Clear();
    componentsToInit.Clear();
    componentListToRemove.Clear();
}

Actor *ActorManager::CreateActor(ActorTag tag)
{
    actors.Push(Actor());
    Actor* actor = &actors[actors.size - 1];
    actor->tag = tag;
    actor->actorManager = this;
    actor->id = actorGeneration++;
    actorsComponentsMap.Add(actor->id, {});
    return actor;
}

static i32 GetChildElementCount(tinyxml2::XMLElement* parent) {
    int count = 0;
    for (tinyxml2::XMLElement* child = parent->FirstChildElement();
        child != nullptr;
        child = child->NextSiblingElement())
    {
        ++count;
    }
    return count;
}

Actor *ActorManager::CreateActorFromFile(const char* filepath)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);

    ActorTag tag = ActorTag::Default;

    tinyxml2::XMLElement* root = doc.FirstChildElement("Actor");
    const char* tagName = 0;
    root->QueryStringAttribute("tag", &tagName);
    if (tagName)
    {
        tag = Actor::TagNameToTag(tagName);
    }

    i32 componentCount = GetChildElementCount(root) + 2; // TODO: handle this better
    if ((componentCount & (componentCount - 1)) != 0)
    {
        componentCount = Utils::NextPower2(componentCount);
    }

    Actor *actor = CreateActor(tag);
    tinyxml2::XMLElement* component = root->FirstChildElement();
    while (component)
    {
        const char* componentType = component->Value();

        if (strcmp("TransformComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            Vector3 position;
            attributes->QueryFloatAttribute("x", &position.x);
            attributes->QueryFloatAttribute("y", &position.y);
            attributes->QueryFloatAttribute("z", &position.z);
            attributes = attributes->NextSiblingElement();
            Vector3 scale;
            attributes->QueryFloatAttribute("x", &scale.x);
            attributes->QueryFloatAttribute("y", &scale.y);
            attributes->QueryFloatAttribute("z", &scale.z);
            attributes = attributes->NextSiblingElement();
            Vector3 rotation;
            attributes->QueryFloatAttribute("x", &rotation.x);
            attributes->QueryFloatAttribute("y", &rotation.y);
            attributes->QueryFloatAttribute("z", &rotation.z);

            rotation *= 1.0f / 180.0f;
            rotation *= (float)ANT_PI;

            TransformComponent transform;
            transform.position = position;
            transform.scale = scale;
            transform.rotation = rotation;
            AddComponent<TransformComponent>(actor, transform);
        }
        else if (strcmp("CameraComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            Vector3 position;
            attributes->QueryFloatAttribute("x", &position.x);
            attributes->QueryFloatAttribute("y", &position.y);
            attributes->QueryFloatAttribute("z", &position.z);
            attributes = attributes->NextSiblingElement();
            Vector3 direction;
            attributes->QueryFloatAttribute("x", &direction.x);
            attributes->QueryFloatAttribute("y", &direction.y);
            attributes->QueryFloatAttribute("z", &direction.z);
            CameraComponent camera;
            camera.Init(position, direction);
            AddComponent<CameraComponent>(actor, camera);
        }
        else if (strcmp("PlayerControllerComponent", componentType) == 0)
        {
            PlayerControllerComponent playerController;
            // TODO: make this work for variable amount of weapons
            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            if (attributes)
            {
                const char* weaponPath;
                attributes->QueryStringAttribute("path", &weaponPath);
                playerController.weaponActor = CreateActorFromFile(weaponPath);
                WeaponComponent* weapon = actor->GetComponent<WeaponComponent>();
                weapon->weapon = playerController.weaponActor;
            }
            AddComponent<PlayerControllerComponent>(actor, playerController);
        }
        else if (strcmp("RenderComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();

            const char* modelPath = 0;
            attributes->QueryStringAttribute("name", &modelPath);

            attributes = attributes->NextSiblingElement();

            Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
            Array<const char*> materialNames;

            if (attributes->ChildElementCount() > 0)
            {
                materialNames.Init(attributes->ChildElementCount(), SCRATCH_MEMORY);

                tinyxml2::XMLElement* material = attributes->FirstChildElement();
                while (material != nullptr)
                {
                    const char* materialPath = 0;
                    material->QueryStringAttribute("name", &materialPath);
                    materialNames.Push(materialPath);
                    material = material->NextSiblingElement();
                }
            }

            ASSERT(modelPath);

            attributes = attributes->NextSiblingElement();

            Vector3 rotationOffset;
            attributes->QueryFloatAttribute("x", &rotationOffset.x);
            attributes->QueryFloatAttribute("y", &rotationOffset.y);
            attributes->QueryFloatAttribute("z", &rotationOffset.z);

            rotationOffset *= (ANT_PI / 180.0f);

            RenderComponent render;
            render.model = ModelManager::Get()->Get(modelPath);
            ASSERT(materialNames.size <= render.model->GetMeshCount());
            for (i32 i = 0; i < materialNames.size; ++i)
            {
                render.model->SetMaterialAtMeshIndex(i, MaterialManager::Get()->Get(materialNames[i]));
            }
            render.rotationOffset = rotationOffset;
            AddComponent<RenderComponent>(actor, render);

            MemoryManager::Get()->ReleaseFrame(frame);
        }
        else if (strcmp("WeaponRenderComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();

            const char* modelPath = 0;
            attributes->QueryStringAttribute("name", &modelPath);

            attributes = attributes->NextSiblingElement();

            Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
            Array<const char*> materialNames;

            if (attributes->ChildElementCount() > 0)
            {
                materialNames.Init(attributes->ChildElementCount(), SCRATCH_MEMORY);

                tinyxml2::XMLElement* material = attributes->FirstChildElement();
                while (material != nullptr)
                {
                    const char* materialPath = 0;
                    material->QueryStringAttribute("name", &materialPath);
                    materialNames.Push(materialPath);
                    material = material->NextSiblingElement();
                }
            }

            ASSERT(modelPath);

            attributes = attributes->NextSiblingElement();

            Vector3 rotationOffset;
            attributes->QueryFloatAttribute("x", &rotationOffset.x);
            attributes->QueryFloatAttribute("y", &rotationOffset.y);
            attributes->QueryFloatAttribute("z", &rotationOffset.z);

            rotationOffset *= (ANT_PI / 180.0f);

            WeaponRenderComponent render;
            render.model = ModelManager::Get()->Get(modelPath);
            ASSERT(materialNames.size <= render.model->GetMeshCount());
            for (i32 i = 0; i < materialNames.size; ++i)
            {
                render.model->SetMaterialAtMeshIndex(i, MaterialManager::Get()->Get(materialNames[i]));
            }
            render.rotationOffset = rotationOffset;
            AddComponent<WeaponRenderComponent>(actor, render);

            MemoryManager::Get()->ReleaseFrame(frame);
            }
        else if (strcmp("AnimatedRenderComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();

            const char* modelPath = 0;
            attributes->QueryStringAttribute("name", &modelPath);

            attributes = attributes->NextSiblingElement();

            Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
            Array<const char*> materialNames;

            if (attributes->ChildElementCount() > 0)
            {
                materialNames.Init(attributes->ChildElementCount(), SCRATCH_MEMORY);

                tinyxml2::XMLElement* material = attributes->FirstChildElement();
                while (material != nullptr)
                {
                    const char* materialPath = 0;
                    material->QueryStringAttribute("name", &materialPath);
                    materialNames.Push(materialPath);
                    material = material->NextSiblingElement();
                }
            }

            ASSERT(modelPath);

            attributes = attributes->NextSiblingElement();

            Vector3 rotationOffset;
            attributes->QueryFloatAttribute("x", &rotationOffset.x);
            attributes->QueryFloatAttribute("y", &rotationOffset.y);
            attributes->QueryFloatAttribute("z", &rotationOffset.z);

            rotationOffset *= (ANT_PI / 180.0f);

            AnimatedRenderComponent render;
            render.model = ModelManager::Get()->Get(modelPath);
            ASSERT(materialNames.size <= render.model->GetMeshCount());
            for (i32 i = 0; i < materialNames.size; ++i)
            {
                render.model->SetMaterialAtMeshIndex(i, MaterialManager::Get()->Get(materialNames[i]));
            }
            render.rotationOffset = rotationOffset;
            AddComponent<AnimatedRenderComponent>(actor, render);

            MemoryManager::Get()->ReleaseFrame(frame);
        }
        else if (strcmp("TiledRenderComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();

            const char* modelPath = 0;
            attributes->QueryStringAttribute("name", &modelPath);

            attributes = attributes->NextSiblingElement();

            Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
            Array<const char*> materialNames;

            if (attributes->ChildElementCount() > 0)
            {
                materialNames.Init(attributes->ChildElementCount(), SCRATCH_MEMORY);

                tinyxml2::XMLElement* material = attributes->FirstChildElement();
                while (material != nullptr)
                {
                    const char* materialPath = 0;
                    material->QueryStringAttribute("name", &materialPath);
                    materialNames.Push(materialPath);
                    material = material->NextSiblingElement();
                }
            }

            ASSERT(modelPath);

            attributes = attributes->NextSiblingElement();

            Vector3 rotationOffset;
            attributes->QueryFloatAttribute("x", &rotationOffset.x);
            attributes->QueryFloatAttribute("y", &rotationOffset.y);
            attributes->QueryFloatAttribute("z", &rotationOffset.z);

            rotationOffset *= (ANT_PI / 180.0f);

            TiledRenderComponent render;
            render.SetModel(ModelManager::Get()->Get(modelPath));
            ASSERT(materialNames.size <= render.GetModel()->GetMeshCount());
            for (i32 i = 0; i < materialNames.size; ++i)
            {
                render.GetModel()->SetMaterialAtMeshIndex(i, MaterialManager::Get()->Get(materialNames[i]));
            }
            render.SetRotationOffset(rotationOffset);
            AddComponent<TiledRenderComponent>(actor, render);

            MemoryManager::Get()->ReleaseFrame(frame);
            }
        else if (strcmp("PhysicsComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            Vector3 acceleration;
            attributes->QueryFloatAttribute("x", &acceleration.x);
            attributes->QueryFloatAttribute("y", &acceleration.y);
            attributes->QueryFloatAttribute("z", &acceleration.z);
            attributes = attributes->NextSiblingElement();
            Vector3 velocity;
            attributes->QueryFloatAttribute("x", &velocity.x);
            attributes->QueryFloatAttribute("y", &velocity.y);
            attributes->QueryFloatAttribute("z", &velocity.z);

            PhysicsComponent physics;
            physics.acceleration = acceleration;
            physics.velocity = velocity;
            physics.forceAccumulator = Vector3(0.0f);
            AddComponent(actor, physics);
        }
        else if (strcmp("WeaponComponent", componentType) == 0)
        {
            // TODO use scratch memory for this
            WeaponComponent* weapon = new WeaponComponent();
            AddComponent<WeaponComponent>(actor, *weapon);
            delete weapon;
        }
        else if (strcmp("EnemyComponent", componentType) == 0)
        {
            EnemyComponent enemy;
            AddComponent<EnemyComponent>(actor, enemy);
        }
        else if (strcmp("AnchorComponent", componentType) == 0)
        {
            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            Vector3 offset;
            attributes->QueryFloatAttribute("x", &offset.x);
            attributes->QueryFloatAttribute("y", &offset.y);
            attributes->QueryFloatAttribute("z", &offset.z);

            AnchorComponent anchor;
            anchor.offset = offset;
            AddComponent<AnchorComponent>(actor, anchor);
        }
        else if (strcmp("ColliderComponents", componentType) == 0)
        {
            TransformComponent* transform = actor->GetComponent<TransformComponent>();

            i32 colliderCount = GetChildElementCount(component) - 1;

            tinyxml2::XMLElement* attributes = component->FirstChildElement();
            Vector3 offset;
            attributes->QueryFloatAttribute("x", &offset.x);
            attributes->QueryFloatAttribute("y", &offset.y);
            attributes->QueryFloatAttribute("z", &offset.z);

            ColliderComponent colliderComponent;
            colliderComponent.Init(colliderCount, memoryType);
            colliderComponent.SetOffset(offset);

            tinyxml2::XMLElement* colliders = attributes->NextSiblingElement();
            while(colliders)
            {

                tinyxml2::XMLElement* collidersAttrib = colliders->FirstChildElement();

                ColliderType colliderType;
                collidersAttrib->QueryIntAttribute("colliderType", (int*)&colliderType);
                collidersAttrib = collidersAttrib->NextSiblingElement();

                Collider collider;
                switch (colliderType)
                {
                case ColliderType::MESH_COLLIDER:
                {
                    const char* filepath;
                    collidersAttrib->QueryStringAttribute("filepath", &filepath);

                    MeshCollider meshCollider;
                    meshCollider.InitFromFile(filepath);
                    collider = Collider(meshCollider, actor);
                } break;
                case ColliderType::CAPSULE:
                {
                    float halfHeight, radio;
                    collidersAttrib->QueryFloatAttribute("halfHeight", &halfHeight);
                    collidersAttrib->QueryFloatAttribute("radio", &radio);

                    Vector3 halfHeightAxis = Vector3(0, halfHeight, 0);

                    Capsule capsule;
                    capsule.Init(
                        (transform->position - halfHeightAxis) + offset,
                        (transform->position + halfHeightAxis) + offset,
                        radio);

                    collider = Collider(capsule, actor);
                } break;
                case ColliderType::AABB:
                {
                    float scaleX, scaleY, scaleZ;
                    collidersAttrib->QueryFloatAttribute("x", &scaleX);
                    collidersAttrib->QueryFloatAttribute("y", &scaleY);
                    collidersAttrib->QueryFloatAttribute("z", &scaleZ);
                    Vector3 scale = Vector3(scaleX, scaleY, scaleZ);
                    AABB aabb;
                    aabb.Init(
                        scale * -0.5f + offset,
                        scale * 0.5f + offset);
                    collider = Collider(aabb, actor);
                }
                }
                colliderComponent.AddSubCollider(collider);

                colliders = colliders->NextSiblingElement();
            }

            AddComponent<ColliderComponent>(actor, colliderComponent);
        }
        else if (strcmp("PortalComponent", componentType) == 0)
        {
            PortalComponent portalComponent;
            AddComponent<PortalComponent>(actor, portalComponent);
        }
        else
        {
            ASSERT(!"Component not Supported!");
        }

        component = component->NextSiblingElement();
    }
    return actor;
}

void ActorManager::InitializeNewComponents()
{
    for (i32 i = 0; i < componentsToInit.size; ++i)
    {
        componentsToInit[i]->OnInit(this);
        componentsToInit[i]->initialized = true;
    }
    componentsToInit.Clear();
}

