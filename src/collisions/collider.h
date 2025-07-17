#pragma once

#include "ray.h"
#include "segment.h"
#include "aabb.h"
#include "obb.h"
#include "sphere.h"
#include "capsule.h"

enum class ColliderType
{
    AABB,
    OBB,
    SPHERE,
    CAPSULE
};

class Collider
{
private:
    ColliderType type;
    union
    {
        AABB aabb;
        OBB obb;
        Sphere sphere;
        Capsule capsule;
    };
public:
    bool Intersect(const Ray& ray, float& t) const;
    bool Intersect(const Segment& segment, float& t) const;
    bool Intersect(const Collider& other) const;
};


