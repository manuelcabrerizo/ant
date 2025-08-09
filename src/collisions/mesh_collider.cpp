#include "mesh_collider.h"
#include "collision_utils.h"
#include <collision.h>
#include "sphere.h"

bool MeshCollider::Intersect(const AABB& aabb, Array<CollisionData>& collisionData) const
{
    // TODO: implement this function
    return false;
}

bool MeshCollider::Intersect(const OBB& obb, Array<CollisionData>& collisionData) const
{
    for (i32 i = 0; i < triangles.size; ++i)
    {
        Vector3 color = Vector3(0, 1, 0);
        CollisionData cd;
        if (obb.Intersect(triangles[i], &cd))
        {
            color = Vector3(1, 0, 0);
            if (collisionData.size < MAX_COLLISION_COUNT)
            {
                collisionData.Push(cd);
            }
        }
        triangles[i].DebugDraw(color);
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return collisionData.size > 0;
}

bool MeshCollider::Intersect(const Sphere& sphere, Array<CollisionData>& collisionData) const
{
    for (int i = 0; i < triangles.size; ++i)
    {
        f32 penetration;
        Vector3 n;
        if (sphere.Intersect(triangles[i], n, penetration))
        {
            if (collisionData.size < MAX_COLLISION_COUNT)
            {
                CollisionData cd;
                cd.n = n;
                cd.penetration = penetration;
                collisionData.Push(cd);
            }
        }
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return collisionData.size > 0;
}

bool MeshCollider::Intersect(const Capsule& capsule, Array<CollisionData>& collisionData) const
{

    for (i32 i = 0; i < triangles.size; ++i)
    {
        f32 penetration;
        Vector3 n;
        if (capsule.Intersect(triangles[i], n, penetration))
        {
            if (collisionData.size < MAX_COLLISION_COUNT)
            {
                CollisionData cd;
                cd.n = n;
                cd.penetration = penetration;
                collisionData.Push(cd);
            }
        }
    }
    CollisionUtils::SortCollisionByPenetration(collisionData);
    return collisionData.size > 0;
}

const Array<Triangle>& MeshCollider::GetTriangles() const
{
    return triangles;
}

