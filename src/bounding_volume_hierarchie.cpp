#include "bounding_volume_hierarchie.h"

#include <collisions/collider.h>
#include <collisions/capsule.h>
#include <collisions/segment.h>

BVHNode::BVHNode(BVHNode* parent, const AABB& volume,
    Triangle* triangle)
    : parent(parent), volume(volume), triangle(triangle) 
{
    childern[0] = nullptr;
    childern[1] = nullptr;
}

BVHNode::~BVHNode()
{
}

bool BVHNode::IsLeaf() const
{
    return triangle != nullptr;
}

bool BVHNode::Overlap(BVHNode* other) const
{
    return volume.Intersect(other->volume);
}

void BVHNode::Insert(const AABB& newVolume, Triangle* newTriangle)
{
    // If we are a leaf, then the only option is to spawn two new childrens
    // and place the new body in one
    if (IsLeaf())
    {
        // Child one is a copy of us
        void* buffer0 = MemoryManager::Get()->Alloc(sizeof(BVHNode), FRAME_MEMORY);
        childern[0] = new (buffer0) BVHNode(this, volume, triangle);

        // Child two holds the new triangle
        void* buffer1 = MemoryManager::Get()->Alloc(sizeof(BVHNode), FRAME_MEMORY);
        childern[1] = new (buffer1) BVHNode(this, newVolume, newTriangle);

        // And we lose the triangle (no longer a leaf)
        triangle = nullptr;

        RecalculateVolume();
    }
    else
    {
        // Otherwise, we need to work out witch child gets to keep
        // the inseted triangle, we give it to whoever would grow the
        // least to incorporate it
        if (childern[0]->volume.GetGrowth(newVolume) <
            childern[1]->volume.GetGrowth(newVolume))
        {
            childern[0]->Insert(newVolume, newTriangle);
        }
        else
        {
            childern[1]->Insert(newVolume, newTriangle);
        }
    }
}

void BVHNode::RecalculateVolume(bool recursive)
{
    if (IsLeaf())
    {
        return;
    }

    volume.Init(childern[0]->volume, childern[1]->volume);

    if (parent)
    {
        parent->RecalculateVolume(true);
    }
}

bool BVHNode::Intersect(const Segment& segment, float& t) const
{
    if (IsLeaf())
    {
        float tOut;
        bool result = segment.Intersect(*triangle, tOut);
        if (tOut < t)
        {
            t = tOut;
        }
        return result;
    }
    bool isIntersecting = false;

    float tempT;
    if (segment.Intersect(volume, tempT))
    {
        isIntersecting |= childern[0]->Intersect(segment, t);
        isIntersecting |= childern[1]->Intersect(segment, t);
    }
    else
    {
        return false;
    }
    return isIntersecting;
}


bool BVHNode::Intersect(const AABB& bVolume, const AABB& aabb, Array<CollisionData>* collisionData) const
{
    if (IsLeaf())
    {
        return aabb.Intersect(*triangle, collisionData);
    }
    bool isIntersecting = false;
    if (bVolume.Intersect(volume))
    {
        isIntersecting |= childern[0]->Intersect(bVolume, aabb, collisionData);
        isIntersecting |= childern[1]->Intersect(bVolume, aabb, collisionData);
    }
    else
    {
        return false;
    }
    return isIntersecting;
}

bool BVHNode::Intersect(const AABB& bVolume, const OBB& obb, Array<CollisionData>* collisionData) const
{
    if (IsLeaf())
    {
        return obb.Intersect(*triangle, collisionData);
    }

    bool isIntersecting = false;
    if (bVolume.Intersect(volume))
    {
        isIntersecting |= childern[0]->Intersect(bVolume, obb, collisionData);
        isIntersecting |= childern[1]->Intersect(bVolume, obb, collisionData);
    }
    else
    {
        return false;
    }
    return isIntersecting;
}
bool BVHNode::Intersect(const AABB& bVolume, const Sphere& sphere, Array<CollisionData>* collisionData) const
{
    if (IsLeaf())
    {
        return sphere.Intersect(*triangle, collisionData);
    }

    bool isIntersecting = false;
    if (bVolume.Intersect(volume))
    {
        isIntersecting |= childern[0]->Intersect(bVolume, sphere, collisionData);
        isIntersecting |= childern[1]->Intersect(bVolume, sphere, collisionData);
    }
    else
    {
        return false;
    }
    return isIntersecting;
}
bool BVHNode::Intersect(const AABB& bVolume, const Capsule& capsule, Array<CollisionData>* collisionData) const
{
    if (IsLeaf())
    {
        return capsule.Intersect(*triangle, collisionData);
    }

    bool isIntersecting = false;
    if (bVolume.Intersect(volume))
    {
        isIntersecting |= childern[0]->Intersect(bVolume, capsule, collisionData);
        isIntersecting |= childern[1]->Intersect(bVolume, capsule, collisionData);
    }
    else
    {
        return false;
    }
    return isIntersecting;
}