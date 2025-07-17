#include "capsule.h"
#include "sphere.h"
#include "ray.h"
#include "segment.h"
#include "triangle.h"
#include "plane.h"
#include "aabb.h"
#include "obb.h"

#include <graphics_manager_d3d11.h>
#include <math/algebra.h>
#include <math/quaternion.h>


void Capsule::Init(Vector3 p, Vector3 q, f32 r)
{
    this->a = p;
    this->b = q;
    this->r = r;
}

Vector3 Capsule::GetA() const
{
    return a;
}

Vector3 Capsule::GetB() const
{
    return b;
}

float Capsule::GetRadio() const
{
    return r;
}

bool Capsule::Intersect(const Capsule& capsule) const
{
    Segment segmentA;
    segmentA.Init(a, b);

    Segment segmentB;
    segmentB.Init(capsule.a, capsule.b);

    Vector3 c1, c2;
    float s, t;
    float distSq = segmentA.ClosestPoint(segmentB, c1, s, c2, t);
    float rSum = r + capsule.r;
    return distSq <= rSum * rSum;
}

bool Capsule::Intersect(const Sphere& sphere) const
{
    Segment axis;
    axis.Init(a, b);
    float t;
    Vector3 center = sphere.GetCenter();
    Vector3 closest = axis.ClosestPoint(center, t);
    float rSum = r + sphere.GetRadio();
    return (closest - center).MagnitudeSq() <= rSum * rSum;
}

bool Capsule::Intersect(const Triangle& triangle, Vector3& n, float& penetration) const
{
    Plane plane;
    plane.Init(triangle);
    float t = (plane.d - Vector3::Dot(plane.n, a)) / Vector3::Dot(plane.n, (b - a));

    Vector3 closestOnPlane = a + (b - a) * t;
    Vector3 closestOnTriangle = triangle.ClosestPoint(closestOnPlane);

    Segment axis;
    axis.Init(a, b);
    Vector3 closestOnSegment = axis.ClosestPoint(closestOnTriangle, t);

    Sphere sphere;
    sphere.Init(closestOnSegment, r);
    return sphere.Intersect(triangle, n, penetration);
}

bool Capsule::Intersect(const Plane& plane) const
{
    Sphere aSphere;
    aSphere.Init(a, r);
    Sphere bSphere;
    bSphere.Init(b, r);
    return aSphere.Intersect(plane) || bSphere.Intersect(plane);
}

bool Capsule::Intersect(const AABB& aabb) const
{
    return aabb.Intersect(*this);
}

bool Capsule::Intersect(const OBB& obb, CollisionData* collisionData) const
{
    return obb.Intersect(*this, collisionData);
}

Vector3 Capsule::ClosestPoint(const Vector3& point) const
{
    Segment axis;
    axis.Init(a, b);
    float t;
    Vector3 closestInSegment = axis.ClosestPoint(point, t);

    Vector3 delta = point - closestInSegment;
    if (delta.MagnitudeSq() < r * r)
    {
        return point;
    }

    Vector3 dir = (point - closestInSegment).Normalized();
    return closestInSegment + dir * r;
}

float Capsule::SqDistPoint(const Vector3& point) const
{
    Vector3 closest = ClosestPoint(point);
    float sqDist = Vector3::Dot(closest - point, closest - point);
    return sqDist;
}

void Capsule::DebugDraw(int subdivision, const Vector3& color)
{
    Vector3 p = this->a;
    Vector3 q = this->b;

    float inc = (2.0f * ANT_PI) / (float)subdivision;

    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 axis = (q - p).Normalized();
    if (fabsf(axis.Dot(up)) > 0.5f)
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
    GraphicsManager::Get()->DebugDrawSphere(p, r, subdivision, subdivision, color);
    GraphicsManager::Get()->DebugDrawSphere(q, r, subdivision, subdivision, color);
}