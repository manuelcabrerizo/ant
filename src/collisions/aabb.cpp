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
#include <cmath> 
#include <algorithm>

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

    float extraPad = 0.01f;

    this->min.x = std::nextafterf(points[xIndexMin].x, -INFINITY) - extraPad;
    this->min.y = std::nextafterf(points[yIndexMin].y, -INFINITY) - extraPad;
    this->min.z = std::nextafterf(points[zIndexMin].z, -INFINITY) - extraPad;
    this->max.x = std::nextafterf(points[xIndexMax].x, INFINITY) + extraPad;
    this->max.y = std::nextafterf(points[yIndexMax].y, INFINITY) + extraPad;
    this->max.z = std::nextafterf(points[zIndexMax].z, INFINITY) + extraPad;
}

void AABB::Init(const AABB& one, const AABB& two)
{
    this->min.x = std::min(one.min.x, two.min.x);
    this->min.y = std::min(one.min.y, two.min.y);
    this->min.z = std::min(one.min.z, two.min.z);
    this->max.x = std::max(one.max.x, two.max.x);
    this->max.y = std::max(one.max.y, two.max.y);
    this->max.z = std::max(one.max.z, two.max.z);
}

void AABB::Init(const Sphere& sphere)
{
    Vector3 halfExtent = Vector3(sphere.GetRadio());
    this->min = sphere.GetCenter() - halfExtent;
    this->max = sphere.GetCenter() + halfExtent;
}

void AABB::Init(const Capsule& capsule)
{
    Sphere aSphere;
    aSphere.Init(capsule.GetA(), capsule.GetRadio());
    Sphere bSphere;
    bSphere.Init(capsule.GetB(), capsule.GetRadio());

    AABB a;
    a.Init(aSphere);
    AABB b;
    b.Init(bSphere);

    Init(a, b);
}

void AABB::Init(const OBB& obb)
{
    Vector3 c = obb.GetCenter();
    Vector3 e = obb.GetExtent();
    Vector3 u[3] = { 
        obb.GetOrientation(0),
        obb.GetOrientation(1),
        obb.GetOrientation(2)
    };

    Vector3 FLB = u[0] * -e.x + u[1] * -e.y + u[2] * -e.z;
    Vector3 FLT = u[0] * -e.x + u[1] * e.y + u[2] * -e.z;
    Vector3 FRT = u[0] * e.x + u[1] * e.y + u[2] * -e.z;
    Vector3 FRB = u[0] * e.x + u[1] * -e.y + u[2] * -e.z;

    Vector3 BLB = u[0] * -e.x + u[1] * -e.y + u[2] * e.z;
    Vector3 BLT = u[0] * -e.x + u[1] * e.y + u[2] * e.z;
    Vector3 BRT = u[0] * e.x + u[1] * e.y + u[2] * e.z;
    Vector3 BRB = u[0] * e.x + u[1] * -e.y + u[2] * e.z;

    FLB += c;
    FLT += c;
    FRT += c;
    FRB += c;

    BLB += c;
    BLT += c;
    BRT += c;
    BRB += c;

    Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
    Array<Vector3> vertices;
    vertices.Init(8, SCRATCH_MEMORY);
    vertices.Push(FLB); vertices.Push(FLT); vertices.Push(FRT); vertices.Push(FRB);
    vertices.Push(BLB); vertices.Push(BLT); vertices.Push(BRT); vertices.Push(BRB);
    this->Init(vertices);
    MemoryManager::Get()->ReleaseFrame(frame);
}

void AABB::Init(const MeshCollider& meshCollider)
{
    *this = meshCollider.GetRootVolume();
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

float AABB::GetVolume() const
{
    float width = max.x - min.x;
    float height = max.y - min.y;
    float depth = max.z - min.z;
    return width * height * depth;
}

float AABB::GetGrowth(const AABB& aabb) const
{
    AABB combine;
    combine.Init(*this, aabb);
    return combine.GetVolume() - GetVolume();
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