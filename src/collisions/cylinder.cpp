#include "cylinder.h"
#include <graphics_manager.h>
#include <math/algebra.h>
#include <math/quaternion.h>

#include "segment.h"
#include "plane.h"


void Cylinder::Init(Vector3 p, Vector3 q, f32 r)
{
    this->p = p;
    this->q = q;
    this->r = r;
}

Vector3 Cylinder::GetP() const
{
    return p;
}

Vector3 Cylinder::GetQ() const
{
    return q;
}

float Cylinder::GetRadio() const
{
    return r;
}

bool Cylinder::Intersect(const Cylinder& cylinder) const
{
    Segment segmentA;
    segmentA.Init(p, q);

    Segment segmentB;
    segmentB.Init(cylinder.p, cylinder.q);

    Vector3 c1, c2;
    float s, t;
    float distSq = segmentA.ClosestPoint(segmentB, c1, s, c2, t);
    float rSum = r + cylinder.r;
    if (distSq > rSum * rSum)
    {
        // early out
        return false;
    }

    Vector3 pq = q - p;
    float t1 = (c1 - p).Dot(pq) / pq.Dot(pq);

    Vector3 closest;
    return false;
}

Vector3 Cylinder::ClosestPoint(const Vector3& point) const
{
    Vector3 pq = q - p;
    float t = (point - p).Dot(pq) / pq.Dot(pq);

    Vector3 normal = pq.Normalized();
    if (t < 0.0f)
    {
        Plane pCap;
        pCap.Init(p, normal);
        Vector3 closest = pCap.ClosestPoint(point);
        if ((closest - p).MagnitudeSq() > r * r)
        {
            closest = p + (closest - p).Normalized() * r;
        }
        return closest;
    }
    else if (t > 1.0f)
    {
        Plane qCap;
        qCap.Init(q, normal * -1.0f);
        Vector3 closest = qCap.ClosestPoint(point);
        if ((closest - q).MagnitudeSq() > r * r)
        {
            closest = q + (closest - q).Normalized() * r;
        }
        return closest;
    }
    else
    {
        Vector3 closestToAxis = p + pq * t;
        Vector3 delta = point - closestToAxis;
        if (delta.MagnitudeSq() < r * r)
        {
            return point;
        }
        Vector3 dir = (point - closestToAxis).Normalized();
        return closestToAxis + dir * r;
    }
}

float Cylinder::SqDistPoint(const Vector3& point) const
{
    Vector3 closest = ClosestPoint(point);
    float sqDist = Vector3::Dot(closest - point, closest - point);
    return sqDist;
}

void Cylinder::DebugDraw(int subdivision, const Vector3& color)
{
    float inc = (2.0f * ANT_PI) / (float)subdivision;

    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 axis = (q - p).Normalized();
    if(fabsf(axis.Dot(up)) > 0.5f)
    {
        up = Vector3(1.0f, 0.0f, 0.0f);
    }

    Vector3 current = axis.Cross(up).Normalized();
    for (i32 i = 0; i < subdivision; ++i)
    {
        Vector3 a = current * r;
        current = Quaternion::AngleAxis(inc, axis) * current;
        Vector3 b = current * r;
        GraphicsManager::Get()->DebugDrawLine(p + a, p + b, color);
        GraphicsManager::Get()->DebugDrawLine(q + a, q + b, color);
        GraphicsManager::Get()->DebugDrawLine(p + a, q + a, color);
        GraphicsManager::Get()->DebugDrawLine(p + b, q + b, color);
    }
}