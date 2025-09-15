#include "key_component.h"
#include <collisions/aabb.h>
#include "transform_component.h"
#include "collider_component.h"
#include "render_component.h"
#include <collision.h>
#include <math/algebra.h>

void KeyComponent::OnInit(ActorManager* actorManager)
{
    transform = owner->GetComponent<TransformComponent>();

    Vector3 size = Vector3(0.75f, 0.25, 0.75);
    AABB aabb;
    aabb.Init(size * -0.5f, size * 0.5f);
    trigger = Collider(aabb, owner);
    trigger.SetOffset(Vector3(0, 0, 0));
    trigger.UpdatePosition(transform->position);
}

void KeyComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    transform->direction = Matrix4::TransformVector(Matrix4::RotateY(timer), Vector3::forward);
    if (timer > (ANT_PI * 2.0f))
    {
        timer = 0;
    }
    timer += dt;

    isGrabbed = false;

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, SCRATCH_MEMORY);
    CollisionWorld::Get()->Intersect(trigger, collisionData);
    for (int i = 0; i < collisionData.size; i++)
    {
        if (collisionData[i].collider->owner->GetTag() == ActorTag::Player)
        {
            isGrabbed = true;
            if (isGrabbed && !wasGrabbed)
            {
                OnGrabbed();
            }
            break;
        }
    }
    MemoryManager::Get()->ReleaseFrame(frame);

    wasGrabbed = isGrabbed;

    trigger.DebugDraw();
}

void KeyComponent::OnGrabbed()
{
    RenderComponent* render = owner->GetComponent<RenderComponent>();
    render->enable = false;
}