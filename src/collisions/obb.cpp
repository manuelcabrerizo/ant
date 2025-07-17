#include "obb.h"
#include <math/algebra.h>

#include <graphics_manager.h>
#include <collision.h>

#include "aabb.h"
#include "plane.h"
#include "sphere.h"
#include "capsule.h"
#include "segment.h"

#include <cfloat>

float OBB::TransformToAxis(const OBB& obb, const Vector3& axis)
{
    return
        obb.e.x * fabsf(axis.Dot(obb.u[0])) +
        obb.e.y * fabsf(axis.Dot(obb.u[1])) +
        obb.e.z * fabsf(axis.Dot(obb.u[2]));
}

float OBB::PenetraionOnAxis(const OBB& one, const OBB& two,
    const Vector3& axis, const Vector3& toCenter)
{
    float oneProject = TransformToAxis(one, axis);
    float twoProject = TransformToAxis(two, axis);
    float distance = fabsf(toCenter.Dot(axis));
    // Return overlap,
    // positive indicates overlap,
    // negative indicates separation
    return oneProject + twoProject - distance;
}

bool OBB::TryAxis(const OBB& one, const OBB& two,
    Vector3 axis, const Vector3& toCenter, int index,
    float& smallestPrenetration, int& smallestCase)
{
    // Make sure we have a normalized axis, dont check almost parallel axes
    if (axis.MagnitudeSq() < 0.0001) return true;
    axis.Normalize();

    float penetration = PenetraionOnAxis(one, two, axis, toCenter);

    if (penetration < 0) return false;
    if (penetration < smallestPrenetration)
    {
        smallestPrenetration = penetration;
        smallestCase = index;
    }

    return true;
}

Vector3 OBB::ContactPoint(
    const Vector3& pOne, const Vector3& dOne, float oneSize,
    const Vector3& pTwo, const Vector3& dTwo, float twoSize,
    bool useOne)
{
    Vector3 toSt, cOne, cTwo;
    float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
    float denom, mua, mub;

    smOne = dOne.MagnitudeSq();
    smTwo = dTwo.MagnitudeSq();
    dpOneTwo = Vector3::Dot(dTwo, dOne);

    toSt = pOne - pTwo;
    dpStaOne = Vector3::Dot(dOne, toSt);
    dpStaTwo = Vector3::Dot(dTwo, toSt);

    denom = smOne * smTwo - dpOneTwo * dpOneTwo;

    // Zero denominator indicates parrallel lines
    if (fabsf(denom) < 0.0001f)
    {
        return useOne ? pOne : pTwo;
    }

    mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
    mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

    // If either of the edges has the nearest point out
    // of bounds, then the edges aren't crossed, we have
    // an edge-face contact. Our point is on the edge, which
    // we know from the useOne parameter.
    if (mua > oneSize ||
        mua < -oneSize ||
        mub > twoSize ||
        mub < -twoSize)
    {
        return useOne ? pOne : pTwo;
    }
    else
    {
        cOne = pOne + dOne * mua;
        cTwo = pTwo + dTwo * mub;

        return cOne * 0.5 + cTwo * 0.5;
    }
}

void OBB::Init(const Vector3& center, const Vector3 orientation[], const Vector3& extent)
{
    this->c = center;
    this->u[0] = orientation[0];
    this->u[1] = orientation[1];
    this->u[2] = orientation[2];
    this->e = extent;
}

Vector3 OBB::GetCenter() const
{
    return c;
}

Vector3 OBB::GetOrientation(int axis) const
{
    return u[axis];
}

Vector3 OBB::GetExtent() const
{
    return e;
}

#define CHECK_OVERLAP(axis, index) \
    if (!TryAxis(one, two, (axis), toCenter, (index), pen, best)) return false;

bool OBB::Intersect(const OBB& two, CollisionData* collisionData) const
{
    const OBB& one = *this;

    Vector3 toCenter = two.c - one.c;

    float pen = FLT_MAX;
    int best = INT_MAX;

    // Now we check each axes, returning if it gives us
    // a separating axis, and keeping track of the axis with
    // the smallest penetration otherwise.
    CHECK_OVERLAP(one.u[0], 0);
    CHECK_OVERLAP(one.u[1], 1);
    CHECK_OVERLAP(one.u[2], 2);

    CHECK_OVERLAP(two.u[0], 3);
    CHECK_OVERLAP(two.u[1], 4);
    CHECK_OVERLAP(two.u[2], 5);

    // Store the best axis-major, in case we run into almost
    // parallel edge collisions later
    unsigned bestSingleAxis = best;

    CHECK_OVERLAP(Vector3::Cross(one.u[0], two.u[0]), 6);
    CHECK_OVERLAP(Vector3::Cross(one.u[0], two.u[1]), 7);
    CHECK_OVERLAP(Vector3::Cross(one.u[0], two.u[2]), 8);
    CHECK_OVERLAP(Vector3::Cross(one.u[1], two.u[0]), 9);
    CHECK_OVERLAP(Vector3::Cross(one.u[1], two.u[1]), 10);
    CHECK_OVERLAP(Vector3::Cross(one.u[1], two.u[2]), 11);
    CHECK_OVERLAP(Vector3::Cross(one.u[2], two.u[0]), 12);
    CHECK_OVERLAP(Vector3::Cross(one.u[2], two.u[1]), 13);
    CHECK_OVERLAP(Vector3::Cross(one.u[2], two.u[2]), 14);

    ASSERT(best != INT_MAX);

    if (!collisionData)
    {
        return true;
    }

    // We now know there is a collision, and we know which
    // of the axes gave the smallest penetration. We now
    // can deal with it in different ways depending on the case
    if (best < 3)
    {
        // We've got a vertex of box two on a face of box one
        Vector3 normal = one.u[best];
        if (one.u[best].Dot(toCenter) > 0)
        {
            normal *= -1.0f;
        }
        Vector3 vertex = two.e;
        if (two.u[0].Dot(normal) < 0) vertex.x = -vertex.x;
        if (two.u[1].Dot(normal) < 0) vertex.y = -vertex.y;
        if (two.u[2].Dot(normal) < 0) vertex.z = -vertex.z;

        if (collisionData)
        {
            collisionData->n = normal;
            collisionData->penetration = pen;
            collisionData->point = Matrix4::TransformPoint(two.GetTransform(), vertex);
        }

        return true;
    }
    else if (best < 6)
    {
        // We've got a vertex of box one on a face of box two
        Vector3 normal = two.u[best - 3];
        if (two.u[best - 3].Dot(toCenter) > 0)
        {
            normal *= -1.0f;
        }
        Vector3 vertex = one.e;
        if (one.u[0].Dot(normal) < 0) vertex.x = -vertex.x;
        if (one.u[1].Dot(normal) < 0) vertex.y = -vertex.y;
        if (one.u[2].Dot(normal) < 0) vertex.z = -vertex.z;

        if (collisionData)
        {
            collisionData->n = normal;
            collisionData->penetration = pen;
            collisionData->point = Matrix4::TransformPoint(one.GetTransform(), vertex);
        }
        return true;
    }
    else
    {
        // We've got an edge-edge contact, Find out which axes
        best -= 6;
        int oneAxisIndex = best / 3;
        int twoAxisIndex = best % 3;
        Vector3 oneAxis = one.u[oneAxisIndex];
        Vector3 twoAxis = two.u[twoAxisIndex];
        Vector3 axis = oneAxis.Cross(twoAxis);
        axis.Normalize();

        // The axis should point from box one to bos two
        if (axis.Dot(toCenter) > 0) axis *= -1.0f;

        // We have the axes, but not the edges: each axis has 4 edges parallel
        // to it, we need to find which of the 4 for each object. We do
        // that by finding the point in the center of the edge. We know
        // its component in the direction of the box's collision axis is zero
        // (its a mid-point) and we determine which of the extremes in each
        // of the other axes is closest
        Vector3 ptOnOneEdge = one.e;
        Vector3 ptOnTwoEdge = two.e;
        for (int i = 0; i < 3; ++i)
        {
            if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
            else if (one.u[i].Dot(axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

            if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
            else if (two.u[i].Dot(axis) < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
        }

        // Move them into world coordinates (they are already oriented
        // correctly, since thet have been derived from the axes)
        ptOnOneEdge = Matrix4::TransformPoint(one.GetTransform(), ptOnOneEdge);
        ptOnTwoEdge = Matrix4::TransformPoint(one.GetTransform(), ptOnTwoEdge);

        // So we have a point and a direction for the colliding edges
        // we need to find out point of closest approach of the two
        // line-segments
        Vector3 vertex = ContactPoint(
            ptOnOneEdge, oneAxis, one.e[oneAxisIndex],
            ptOnTwoEdge, twoAxis, two.e[twoAxisIndex],
            bestSingleAxis > 2);

        if (collisionData)
        {
            collisionData->n = axis;
            collisionData->penetration = pen;
            collisionData->point = vertex;
        }

        return true;
    }
    return false;
}
#undef CHECK_OVERLAP

bool OBB::Intersect(const AABB& aabb, CollisionData *collisionData) const
{
    // TODO: implement the real version of this funciton
    Vector3 min = aabb.GetMin();
    Vector3 max = aabb.GetMax();

    Vector3 extent = (max - min) * 0.5f;
    Vector3 center = min + extent;
    Vector3 orientation[3] = {
        Vector3(1, 0, 0),
        Vector3(0, 1, 0),
        Vector3(0, 0, 1)
    };

    OBB obb;
    obb.Init(center, orientation, extent);
    return Intersect(obb, collisionData);
}

bool OBB::Intersect(const Plane& plane, CollisionData* collisionData) const
{
    Vector3 vertices[8] =
    {
        (u[0] * -e.x + u[1] * -e.y + u[2] * -e.z) + c,
        (u[0] * -e.x + u[1] * e.y + u[2] * -e.z) + c,
        (u[0] * e.x + u[1] * e.y + u[2] * -e.z) + c,
        (u[0] * e.x + u[1] * -e.y + u[2] * -e.z) + c,
        (u[0] * -e.x + u[1] * -e.y + u[2] * e.z) + c,
        (u[0] * -e.x + u[1] * e.y + u[2] * e.z) + c,
        (u[0] * e.x + u[1] * e.y + u[2] * e.z) + c,
        (u[0] * e.x + u[1] * -e.y + u[2] * e.z) + c
    };

    int contactFound = 0;
    for (int i = 0; i < 8; i++)
    {
        float dist = Vector3::Dot(vertices[i], plane.n);
        if (dist <= plane.d)
        {
            if (collisionData)
            {
                collisionData->point = plane.n;
                collisionData->point *= (dist - plane.d);
                collisionData->point += vertices[i];
                collisionData->n = plane.n;
                collisionData->penetration = plane.d - dist;
            }
            contactFound++;
        }
    }
    return contactFound > 0;
}

bool OBB::Intersect(const Sphere& sphere, CollisionData* collisionData) const
{
    Vector3 center = sphere.GetCenter();
    float r = sphere.GetRadio();

    Vector3 closest = ClosestPoint(center);
    Vector3 toObb = center - closest;
    float lenSq = toObb.MagnitudeSq();

    if (collisionData)
    {
        collisionData->n = toObb.Normalized();
        collisionData->penetration = r - sqrtf(lenSq);
        collisionData->point = closest;
    }

    return lenSq <= r * r;
}

bool OBB::Intersect(const Capsule& capsule, CollisionData* collisionData) const
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



Vector3 OBB::ClosestPoint(const Vector3& point) const
{
    Vector3 d = point - c;
    // start result at center of box, make steps from there
    Vector3 q = c;
    for (int i = 0; i < 3; i++)
    {
        // project d onto that axis to get distance
        // along the axis of d from the box center
        float dist = Vector3::Dot(d, u[i]);
        // is distance farther that the box extents, clamp to the box
        if (dist > e[i]) dist = e[i];
        if (dist < -e[i]) dist = -e[i];
        q += u[i] * dist;
    }
    return q;
}

float OBB::SqDistPoint(const Vector3& point) const
{
    Vector3 closest = ClosestPoint(point);
    float sqDist = Vector3::Dot(closest - point, closest - point);
    return sqDist;
}

void OBB::DebugDraw(const Vector3& color)
{
    Vector3 FLB = u[0] * -e.x + u[1] * -e.y + u[2] * -e.z;
    Vector3 FLT = u[0] * -e.x + u[1] *  e.y + u[2] * -e.z;
    Vector3 FRT = u[0] *  e.x + u[1] *  e.y + u[2] * -e.z;
    Vector3 FRB = u[0] *  e.x + u[1] * -e.y + u[2] * -e.z;

    Vector3 BLB = u[0] * -e.x + u[1] * -e.y + u[2] * e.z;
    Vector3 BLT = u[0] * -e.x + u[1] *  e.y + u[2] * e.z;
    Vector3 BRT = u[0] *  e.x + u[1] *  e.y + u[2] * e.z;
    Vector3 BRB = u[0] *  e.x + u[1] * -e.y + u[2] * e.z;

    FLB += c;
    FLT += c;
    FRT += c;
    FRB += c;

    BLB += c;
    BLT += c;
    BRT += c;
    BRB += c;

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

Matrix4 OBB::GetTransform() const
{
    return Matrix4::TransformFromBasis(c, u[0], u[1], u[2]);
}
