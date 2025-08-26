#pragma once

#include <collisions/aabb.h>

class Capsule;

class BVHNode
{
public:
    BVHNode(BVHNode* parent, const AABB& volume,
        Triangle* triangle = nullptr);
    ~BVHNode();

    // Hold the child nodes of this node
    BVHNode* childern[2] = {nullptr, nullptr};
    BVHNode* parent = nullptr;

    // Hold a single bounding volume encompassing all
    // the descendants of this node
    AABB volume;

    // Hold the triangle at this node of the herarchy
    // Only leaf nodes can have a triangle
    Triangle* triangle = nullptr;

    bool IsLeaf() const;
    bool Overlap(BVHNode* other) const;
    void Insert(const AABB& volume, Triangle* triangle);
    void RecalculateVolume(bool recursive = true);

    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;
};


