#include "collider.h"

#include "ray.h"
#include "segment.h"


Collider::Collider(const AABB& aabb, Actor* actor)
{
    this->owner = actor;
    this->type = ColliderType::AABB;
    this->aabb = aabb;
    this->volume = aabb;
}

Collider::Collider(const OBB& obb, Actor* actor)
{
    this->owner = actor;
    this->type = ColliderType::OBB;
    this->obb = obb;
    this->volume.Init(obb);
}

Collider::Collider(const Sphere& sphere, Actor* actor)
{
    this->owner = actor;
    this->type = ColliderType::SPHERE;
    this->sphere = sphere;
    this->volume.Init(sphere);
}

Collider::Collider(const Capsule& capsule, Actor* actor)
{
    this->owner = actor;
    this->type = ColliderType::CAPSULE;
    this->capsule = capsule;
    this->volume.Init(capsule);
}

Collider::Collider(const MeshCollider& meshCollider, Actor* actor)
{
    this->owner = actor;
    this->type = ColliderType::MESH_COLLIDER;
    this->meshCollider = meshCollider;
    this->volume.Init(meshCollider);
}

bool Collider::Intersect(const Ray& ray, float& t) const
{
    switch (type)
    {
    case ColliderType::AABB:
        return ray.Intersect(aabb, t);
    case ColliderType::OBB:
        return ray.Intersect(obb, t);
    case ColliderType::SPHERE:
        return ray.Intersect(sphere, t);
    case ColliderType::CAPSULE:
        return ray.Intersect(capsule, t);
    case ColliderType::MESH_COLLIDER:
        return meshCollider.Intersect(ray, t);
    }
    return false;
}

bool Collider::Intersect(const Segment& segment, float& t) const
{
    switch (type)
    {
    case ColliderType::AABB:
        return segment.Intersect(aabb, t);
    case ColliderType::OBB:
        return segment.Intersect(obb, t);
    case ColliderType::SPHERE:
        return segment.Intersect(sphere, t);
    case ColliderType::CAPSULE:
        return segment.Intersect(capsule, t);
    case ColliderType::MESH_COLLIDER:
        return meshCollider.Intersect(segment, t);
    }
    return false;
}

bool Collider::Intersect(const Sphere& sphere, Array<CollisionData>& collisionData) const
{
    // early out is the bounding bolumes are not intersecting
    if (!volume.Intersect(sphere))
    {
        return false;
    }

    switch (type)
    {
    case ColliderType::AABB:   return sphere.Intersect(aabb, &collisionData);
    case ColliderType::OBB:    return sphere.Intersect(obb, &collisionData);
    case ColliderType::SPHERE: return sphere.Intersect(sphere, &collisionData);
    case ColliderType::CAPSULE:return sphere.Intersect(capsule, &collisionData);
    }
}


bool Collider::Intersect(const Collider& other, Array<CollisionData>& collisionData) const
{
    // early out is the bounding bolumes are not intersecting
    if (!volume.Intersect(other.volume))
    {
        return false;
    }

    if (other.type == ColliderType::MESH_COLLIDER)
    {
        return other.meshCollider.Intersect(*this, &collisionData);
    }
    else
    {
        switch (type)
        {
        case ColliderType::AABB:
            switch (other.type)
            {
            case ColliderType::AABB:   return aabb.Intersect(other.aabb, &collisionData);
            case ColliderType::OBB:    return aabb.Intersect(other.obb, &collisionData);
            case ColliderType::SPHERE: return aabb.Intersect(other.sphere, &collisionData);
            case ColliderType::CAPSULE:return aabb.Intersect(other.capsule, &collisionData);
            }
            break;

        case ColliderType::OBB:
            switch (other.type)
            {
            case ColliderType::AABB:   return obb.Intersect(other.aabb, &collisionData);
            case ColliderType::OBB:    return obb.Intersect(other.obb, &collisionData);
            case ColliderType::SPHERE: return obb.Intersect(other.sphere, &collisionData);
            case ColliderType::CAPSULE:return obb.Intersect(other.capsule, &collisionData);
            }
            break;

        case ColliderType::SPHERE:
            switch (other.type)
            {
            case ColliderType::AABB:   return sphere.Intersect(other.aabb, &collisionData);
            case ColliderType::OBB:    return sphere.Intersect(other.obb, &collisionData);
            case ColliderType::SPHERE: return sphere.Intersect(other.sphere, &collisionData);
            case ColliderType::CAPSULE:return sphere.Intersect(other.capsule, &collisionData);
            }
            break;

        case ColliderType::CAPSULE:
            switch (other.type)
            {
            case ColliderType::AABB:   return capsule.Intersect(other.aabb, &collisionData);
            case ColliderType::OBB:    return capsule.Intersect(other.obb, &collisionData);
            case ColliderType::SPHERE: return capsule.Intersect(other.sphere, &collisionData);
            case ColliderType::CAPSULE:return capsule.Intersect(other.capsule, &collisionData);
            }
            break;

        case ColliderType::MESH_COLLIDER:
            return meshCollider.Intersect(other, &collisionData);
            break;
        }
        return false; // fallback, shouldn't be reached if all types are handled
    }
}

Actor* Collider::GetOwner() const
{
    return owner;
}

void Collider::UpdatePosition(const Vector3& position)
{
    switch (type)
    {
        case ColliderType::AABB:
        {
            float width = aabb.GetMax().x - aabb.GetMin().x;
            float height = aabb.GetMax().y - aabb.GetMin().y;
            float depth = aabb.GetMax().z - aabb.GetMin().z;
            Vector3 absPos = position + offset;
;           aabb.SetMin(Vector3(absPos.x - width * 0.5f, absPos.y - height * 0.5f, absPos.z - depth * 0.5f));
;           aabb.SetMax(Vector3(absPos.x + width * 0.5f, absPos.y + height * 0.5f, absPos.z + depth * 0.5f));
            volume = aabb;
        } break;
        case ColliderType::OBB:
        {
            Vector3 absPos = position + offset;
            obb.SetCenter(absPos);
            volume.Init(obb);
        } break;
        case ColliderType::SPHERE:
        {
            Vector3 absPos = position + offset;
            sphere.SetCenter(absPos);
            volume.Init(sphere);
        } break;
        case ColliderType::CAPSULE:
        {
            Vector3 absPos = position + offset;
            capsule.SetPosition(absPos);
            volume.Init(capsule);
        } break;
    }
}

void Collider::DebugDraw() const
{
    Vector3 color = Vector3(0, 1, 0);
    switch (type)
    {
        case ColliderType::AABB:
        {
            aabb.DebugDraw(color);
        } break;
        case ColliderType::OBB:
        {
            obb.DebugDraw(color);
        } break;
        case ColliderType::SPHERE:
        {
            //sphere.DebugDraw(12, color);
        } break;
        case ColliderType::CAPSULE:
        {
            //capsule.DebugDraw(6, color);
        } break;
        case ColliderType::MESH_COLLIDER:
        {
            //meshCollider.DebugDraw(color);
        } break;
    }
    volume.DebugDraw(Vector3(0, 0, 1));

}

void Collider::SetOffset(const Vector3& offset)
{
    this->offset = offset;
}