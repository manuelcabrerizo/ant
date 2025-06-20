#include "obb.h"
#include <math/algebra.h>
#include <math/matrix4.h>

#include <graphics_manager.h>

#include "plane.h"
#include "sphere.h"

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

static const float EPSILON = 0.001f;

bool OBB::Intersect(const OBB& b) const
{
    const OBB& a = *this;

    float ra, rb;
    float R[3][3];
    float AbsR[3][3];

    // Compute rotation matrix expressing b in a's coordinate frame
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = Vector3::Dot(a.u[i], b.u[j]);

    // Compute translation vector t
    Vector3 t = b.c - a.c;
    // Bring translation into a's coordinate frame
    t = Vector3(Vector3::Dot(t, a.u[0]), Vector3::Dot(t, a.u[1]), Vector3::Dot(t, a.u[2]));
    
    // Compute common subexpressions. Add an epsilon term to
    // counteract arithmetic errors when two edges are parallel
    // and their cross product is near null
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            AbsR[i][j] = Abs(R[i][j]) + EPSILON;

    // Test axis L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++)
    {
        ra = a.e[i];
        rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
        if (Abs(t[0]) > ra + rb) return false;
    }

    // Test axis L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++)
    {
        ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
        rb = b.e[i];
        if (Abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
    }

    // Test axis L = A0 x B0
    ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
    rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];
    if (Abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

    // Test axis L = A0 x B1
    ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
    rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];
    if (Abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;

    // Test axis L = A0 x B2
    ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
    rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];
    if (Abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

    // Test axis L = A1 x B0
    ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
    rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];
    if (Abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;

    // Test axis L = A1 x B1
    ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
    rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];
    if (Abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;

    // Test axis L = A1 x B2
    ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
    rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];
    if (Abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;

    // Test axis L = A2 x B0
    ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
    rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];
    if (Abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;

    // Test axis L = A2 x B1
    ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
    rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];
    if (Abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;

    // Test axis L = A2 x B2
    ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
    rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];
    if (Abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;

    return true;
}

bool OBB::Intersect(const Plane& plane) const
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
        if (Vector3::Dot(vertices[i], plane.n) <= plane.d)
        {
            contactFound++;
        }
    }
    return contactFound > 0;
}

bool OBB::Intersect(const Sphere& sphere) const
{
    Vector3 center = sphere.GetCenter();
    float r = sphere.GetRadio();
    return SqDistPoint(center) <= r * r;
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
