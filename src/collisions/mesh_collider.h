#pragma once

#include "collision_utils.h"
#include "triangle.h"
#include <bounding_volume_hierarchie.h>

class Collider;

class MeshCollider
{
private:
    Array<Triangle> triangles;
    BVHNode* root = nullptr;

    // this can be a temp array using SCRATCH_MEMORY
    Array<AABB> boundingBoxes;
public:

    void InitFromFile(const char* filepath);
    bool Intersect(const Segment& segment, float& t) const;
    bool Intersect(const Collider& collider, Array<CollisionData>* collisionData = nullptr) const;
    const Array<Triangle>& GetTriangles() const;

    void DebugDraw(const Vector3& color) const;
};

