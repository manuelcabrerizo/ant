#include "aabb.h"

#include "collision_utils.h"

#include "obb.h"
#include "plane.h"
#include "sphere.h"
#include "capsule.h"
#include "segment.h"

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

Vector3 AABB::GetMin() const
{
    return min;
}

Vector3 AABB::GetMax() const
{
    return max;
}

bool AABB::Intersect(const AABB& aabb) const
{
    if (max[0] < aabb.min[0] || min[0] > aabb.max[0]) return false;
    if (max[1] < aabb.min[1] || min[1] > aabb.max[1]) return false;
    if (max[2] < aabb.min[2] || min[2] > aabb.max[2]) return false;
    return true;
}

bool AABB::Intersect(const OBB& obb) const
{
    return obb.Intersect(*this);
}


bool AABB::Intersect(const Plane& plane) const
{
    Vector3 vertices[8] = 
    {
        Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), Vector3(max.x, min.y, min.z),
        Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), Vector3(max.x, max.y, max.z), Vector3(max.x, max.y, min.z)
    };

    int contactFound = 0;
    for (int i = 0; i < 8; i++)
    {
        if (Vector3::Dot(vertices[i], plane.n) <= plane.d)
        {
            contactFound++;
        }
    }
    return contactFound > 0;
}

bool AABB::Intersect(const Capsule& capsule) const
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
    return Intersect(sphere);
}


bool AABB::Intersect(const Sphere& sphere) const
{
    Vector3 center = sphere.GetCenter();
    float r = sphere.GetRadio();
    return SqDistPoint(center) <= r * r;
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


void AABB::DebugDraw(const Vector3& color)
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