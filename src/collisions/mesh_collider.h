#pragma once

#include "collision_utils.h"
#include "triangle.h"
#include <bounding_volume_hierarchie.h>

class AABB;
class OBB;
class Plane;
class Sphere;
class Capsule;

class MeshCollider
{
private:
    Array<Triangle> triangles;
    BVHNode* root = nullptr;

    // this can be a temp array using SCRATCH_MEMORY
    Array<AABB> boundingBoxes;
public:

    void InitFromFile(const char* filepath);

    bool Intersect(const AABB& aabb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const OBB& obb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;

    const Array<Triangle>& GetTriangles() const;

    void DebugDraw(const Vector3& color) const;
};

