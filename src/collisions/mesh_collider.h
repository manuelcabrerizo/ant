#pragma once

#include "collision_utils.h"
#include "triangle.h"

class AABB;
class OBB;
class Plane;
class Sphere;
class Capsule;

class MeshCollider
{
private:
    Array<Triangle> triangles;
public:

    void InitFromFile(const char* filepath);

    bool Intersect(const AABB& aabb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const OBB& obb, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>* collisionData = nullptr) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData>* collisionData = nullptr) const;

    const Array<Triangle>& GetTriangles() const;
};

