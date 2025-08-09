#pragma once
#include <common.h>
#include <containers.h>

#include "triangle.h"

class AABB;
class OBB;
class Plane;
class Sphere;
class Capsule;

struct CollisionData;

class MeshCollider
{
private:
    Array<Triangle> triangles;
public:
    bool Intersect(const AABB& aabb, Array<CollisionData>& collisionData) const;
    bool Intersect(const OBB& obb, Array<CollisionData>& collisionData) const;
    bool Intersect(const Sphere& sphere, Array<CollisionData>& collisionData) const;
    bool Intersect(const Capsule& capsule, Array<CollisionData>& collisionData) const;

    const Array<Triangle>& GetTriangles() const;
};

