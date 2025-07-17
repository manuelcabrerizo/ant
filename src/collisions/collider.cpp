#include "collider.h"

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
    }
    return false;
}

bool Collider::Intersect(const Collider& other) const
{
    switch (type)
    {
    case ColliderType::AABB:
        switch (other.type)
        {
        case ColliderType::AABB:   return aabb.Intersect(other.aabb);
        case ColliderType::OBB:    return aabb.Intersect(other.obb);
        case ColliderType::SPHERE: return aabb.Intersect(other.sphere);
        case ColliderType::CAPSULE:return aabb.Intersect(other.capsule);
        }
        break;

    case ColliderType::OBB:
        switch (other.type)
        {
        case ColliderType::AABB:   return obb.Intersect(other.aabb);
        case ColliderType::OBB:    return obb.Intersect(other.obb);
        case ColliderType::SPHERE: return obb.Intersect(other.sphere);
        case ColliderType::CAPSULE:return obb.Intersect(other.capsule);
        }
        break;

    case ColliderType::SPHERE:
        switch (other.type)
        {
        case ColliderType::AABB:   return sphere.Intersect(other.aabb);
        case ColliderType::OBB:    return sphere.Intersect(other.obb);
        case ColliderType::SPHERE: return sphere.Intersect(other.sphere);
        case ColliderType::CAPSULE:return sphere.Intersect(other.capsule);
        }
        break;

    case ColliderType::CAPSULE:
        switch (other.type)
        {
        case ColliderType::AABB:   return capsule.Intersect(other.aabb);
        case ColliderType::OBB:    return capsule.Intersect(other.obb);
        case ColliderType::SPHERE: return capsule.Intersect(other.sphere);
        case ColliderType::CAPSULE:return capsule.Intersect(other.capsule);
        }
        break;
    }

    return false; // fallback, shouldn't be reached if all types are handled
}

