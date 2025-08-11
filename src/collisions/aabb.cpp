#include "aabb.h"

#include "collision_utils.h"

#include "obb.h"
#include "plane.h"
#include "sphere.h"
#include "capsule.h"
#include "segment.h"
#include "mesh_collider.h"

#include <graphics_manager.h>
#include <cfloat>

void AABB::Init(const Vector3& min, const Vector3& max)
{
    this->min = min;
    this->max = max;
}

void AABB::Init(Array<Vector3>& points)
{
    int xIndexMin, xIndexMax;
    CollisionUtils::ExtremePointsAlongDirection(Vector3(1.0f, 0.0f, 0.0f), points, &xIndexMin, &xIndexMax);
    int yIndexMin, yIndexMax;
    CollisionUtils::ExtremePointsAlongDirection(Vector3(0.0f, 1.0f, 0.0f), points, &yIndexMin, &yIndexMax);
    int zIndexMin, zIndexMax;
    CollisionUtils::ExtremePointsAlongDirection(Vector3(0.0f, 0.0f, 1.0f), points, &zIndexMin, &zIndexMax);

    this->min.x = points[xIndexMin].x;
    this->min.y = points[yIndexMin].y;
    this->min.z = points[zIndexMin].z;

    this->max.x = points[xIndexMax].x;
    this->max.y = points[yIndexMax].y;
    this->max.z = points[zIndexMax].z;
}

void AABB::SetMin(const Vector3& min)
{
    this->min = min;
}

void AABB::SetMax(const Vector3& max)
{
    this->max = max;
}

Vector3 AABB::GetMin() const
{
    return min;
}

Vector3 AABB::GetMax() const
{
    return max;
}

bool AABB::Intersect(const AABB& aabb, Array<CollisionData>* collisionData) const
{
    if (max[0] < aabb.min[0] || min[0] > aabb.max[0]) return false;
    if (max[1] < aabb.min[1] || min[1] > aabb.max[1]) return false;
    if (max[2] < aabb.min[2] || min[2] > aabb.max[2]) return false;
    if (collisionData && collisionData->size < MAX_COLLISION_COUNT)
    {
        OBB obb;
        obb.Init(*this);
        obb.Intersect(aabb, collisionData);
    }
    return true;
}

bool AABB::Intersect(const OBB& obb, Array<CollisionData>* collisionData) const
{
    bool isIntersecting = obb.Intersect(*this, collisionData);
    if (isIntersecting)
    {
        collisionData->data[collisionData->size - 1].n *= -1.0f;
    }
    return isIntersecting;
}

bool AABB::Intersect(const Plane& plane, Array<CollisionData>* collisionData) const
{
    Vector3 vertices[8] = 
    {
        Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), Vector3(max.x, min.y, min.z),
        Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), Vector3(max.x, max.y, max.z), Vector3(max.x, max.y, min.z)
    };

    Vector3 planeNormal = plane.GetNormal();
    float planeDistance = plane.GetDistance();

    int contactFound = 0;
    for (int i = 0; i < 8; i++)
    {
        float dist = Vector3::Dot(vertices[i], planeNormal);
        if (dist <= planeDistance)
        {
            if (collisionData && collisionData->size < MAX_COLLISION_COUNT)
            {
                CollisionData collision;
                collision.point = planeNormal;
                collision.point *= (dist - planeDistance);
                collision.point += vertices[i];
                collision.n = planeNormal;
                collision.penetration = planeDistance - dist;
                collisionData->Push(collision);
            }
            contactFound++;
        }
    }
    return contactFound > 0;
}

bool AABB::Intersect(const Capsule& capsule, Array<CollisionData>* collisionData) const
{
    Vector3 testPoints[3];
    testPoints[0] = capsule.GetA();
    testPoints[1] = capsule.GetB();
    testPoints[2] = testPoints[0] + (testPoints[1] - testPoints[0]) * 0.5f;

    Vector3 closestOnBox;
    float minDistSq = FLT_MAX;
    for (int i = 0; i < 3; ++i)
    {
        Vector3 closest = ClosestPoint(testPoints[i]);
        float distSq = (testPoints[i] - closest).MagnitudeSq();
        if (distSq < minDistSq)
        {
            minDistSq = distSq;
            closestOnBox = closest;
        }
    }

    Segment axis;
    axis.Init(capsule.GetA(), capsule.GetB());
    float t;
    Vector3 closestOnSegment = axis.ClosestPoint(closestOnBox, t);

    Sphere sphere;
    sphere.Init(closestOnSegment, capsule.GetRadio());
    return Intersect(sphere, collisionData);
}


bool AABB::Intersect(const Sphere& sphere, Array<CollisionData>* collisionData) const
{
    Vector3 center = sphere.GetCenter();
    float r = sphere.GetRadio();
    Vector3 closest = ClosestPoint(center);
    Vector3 toAABB = closest - center;
    float distSq = toAABB.MagnitudeSq();
    bool isIntersecting = distSq <= r * r;
    if (isIntersecting && collisionData && collisionData->size < MAX_COLLISION_COUNT)
    {
        CollisionData collision;
        collision.n = toAABB.Normalized();
        collision.penetration = r - sqrtf(distSq);
        collision.point = closest;
        collisionData->Push(collision);
    }
    return  isIntersecting;
}

bool AABB::Intersect(const Triangle& triangle, Array<CollisionData>* collisionData) const
{
    OBB obb;
    obb.Init(*this);
    return obb.Intersect(triangle, collisionData);
}


bool AABB::Intersect(const MeshCollider& meshCollider, Array<CollisionData>* collisionData) const
{
    return meshCollider.Intersect(*this, collisionData);
}

Vector3 AABB::ClosestPoint(const Vector3& point) const
{
    Vector3 q;
    for (int i = 0; i < 3; i++)
    {
        float v = point[i];
        if (v < min[i]) v = min[i];
        if (v > max[i]) v = max[i];
        q[i] = v;
    }
    return q;
}

float AABB::SqDistPoint(const Vector3& point) const
{
    float sqDist = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        float v = point[i];
        if (v < min[i]) sqDist += (min[i] - v) * (min[i] - v);
        if (v > max[i]) sqDist += (v - max[i]) * (v - max[i]);
    }
    return sqDist;
}


void AABB::DebugDraw(const Vector3& color) const
{
    Vector3 FLB = min;
    Vector3 FLT = Vector3(min.x, max.y, min.z);
    Vector3 FRT = Vector3(max.x, max.y, min.z);
    Vector3 FRB = Vector3(max.x, min.y, min.z);

    Vector3 BRT = max;
    Vector3 BLB = Vector3(min.x, min.y, max.z);
    Vector3 BLT = Vector3(min.x, max.y, max.z);
    Vector3 BRB = Vector3(max.x, min.y, max.z);


    // Front face
    GraphicsManager::Get()->DebugDrawLine(FLB, FLT, color);
    GraphicsManager::Get()->DebugDrawLine(FLT, FRT, color);
    GraphicsManager::Get()->DebugDrawLine(FRT, FRB, color);
    GraphicsManager::Get()->DebugDrawLine(FRB, FLB, color);

    // Back face
    GraphicsManager::Get()->DebugDrawLine(BLB, BLT, color);
    GraphicsManager::Get()->DebugDrawLine(BLT, BRT, color);
    GraphicsManager::Get()->DebugDrawLine(BRT, BRB, color);
    GraphicsManager::Get()->DebugDrawLine(BRB, BLB, color);

    // Left face
    GraphicsManager::Get()->DebugDrawLine(BLB, FLB, color);
    GraphicsManager::Get()->DebugDrawLine(FLT, BLT, color);

    // Right face
    GraphicsManager::Get()->DebugDrawLine(BRB, FRB, color);
    GraphicsManager::Get()->DebugDrawLine(FRT, BRT, color);
}