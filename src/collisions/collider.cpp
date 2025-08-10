#include "collider.h"

#include "ray.h"
#include "segment.h"

unsigned int Collider::gen = 1;

Collider::Collider(const AABB& aabb)
{
    this->id = gen++;
    this->type = ColliderType::AABB;
    this->aabb = aabb;
}

Collider::Collider(const OBB& obb)
{
    this->id = gen++;
    this->type = ColliderType::OBB;
    this->obb = obb;
}

Collider::Collider(const Sphere& sphere)
{
    this->id = gen++;
    this->type = ColliderType::SPHERE;
    this->sphere = sphere;
}

Collider::Collider(const Capsule& capsule)
{
    this->id = gen++;
    this->type = ColliderType::CAPSULE;
    this->capsule = capsule;
}

Collider::Collider(const MeshCollider& meshCollider)
{
    this->id = gen++;
    this->type = ColliderType::MESH_COLLIDER;
    this->meshCollider = meshCollider;
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
        return ray.Intersect(meshCollider, t);
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
        return segment.Intersect(meshCollider, t);
    }
    return false;
}

bool Collider::Intersect(const Collider& other, Array<CollisionData>& collisionData) const
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
        case ColliderType::MESH_COLLIDER: return aabb.Intersect(other.meshCollider, &collisionData);
        }
        break;

    case ColliderType::OBB:
        switch (other.type)
        {
        case ColliderType::AABB:   return obb.Intersect(other.aabb, &collisionData);
        case ColliderType::OBB:    return obb.Intersect(other.obb, &collisionData);
        case ColliderType::SPHERE: return obb.Intersect(other.sphere, &collisionData);
        case ColliderType::CAPSULE:return obb.Intersect(other.capsule, &collisionData);
        case ColliderType::MESH_COLLIDER: return obb.Intersect(other.meshCollider, &collisionData);
        }
        break;

    case ColliderType::SPHERE:
        switch (other.type)
        {
        case ColliderType::AABB:   return sphere.Intersect(other.aabb, &collisionData);
        case ColliderType::OBB:    return sphere.Intersect(other.obb, &collisionData);
        case ColliderType::SPHERE: return sphere.Intersect(other.sphere, &collisionData);
        case ColliderType::CAPSULE:return sphere.Intersect(other.capsule, &collisionData);
        case ColliderType::MESH_COLLIDER: return sphere.Intersect(other.meshCollider, &collisionData);
        }
        break;

    case ColliderType::CAPSULE:
        switch (other.type)
        {
        case ColliderType::AABB:   return capsule.Intersect(other.aabb, &collisionData);
        case ColliderType::OBB:    return capsule.Intersect(other.obb, &collisionData);
        case ColliderType::SPHERE: return capsule.Intersect(other.sphere, &collisionData);
        case ColliderType::CAPSULE:return capsule.Intersect(other.capsule, &collisionData);
        case ColliderType::MESH_COLLIDER: return capsule.Intersect(other.meshCollider, &collisionData);
        }
        break;

    case ColliderType::MESH_COLLIDER:
        switch (other.type)
        {
        case ColliderType::AABB:   return meshCollider.Intersect(other.aabb, &collisionData);
        case ColliderType::OBB:    return meshCollider.Intersect(other.obb, &collisionData);
        case ColliderType::SPHERE: return meshCollider.Intersect(other.sphere, &collisionData);
        case ColliderType::CAPSULE:return meshCollider.Intersect(other.capsule, &collisionData);
        }
        break;
    }
    return false; // fallback, shouldn't be reached if all types are handled
}

unsigned int Collider::GetId() const
{
    return id;
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
;           aabb.SetMin(Vector3(position.x - width * 0.5f, position.y - height * 0.5f, position.z - depth * 0.5f));
;           aabb.SetMax(Vector3(position.x + width * 0.5f, position.y + height * 0.5f, position.z + depth * 0.5f));
        } break;
        case ColliderType::OBB:
        {
            obb.SetCenter(position);
        } break;
        case ColliderType::SPHERE:
        {
            sphere.SetCenter(position);
        } break;
        case ColliderType::CAPSULE:
        {
            capsule.SetPosition(position);
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
            sphere.DebugDraw(12, color);
        } break;
        case ColliderType::CAPSULE:
        {
            capsule.DebugDraw(12, color);
        } break;
    }
}