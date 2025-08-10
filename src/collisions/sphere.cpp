#include "sphere.h"
#include <math.h>
#include <float.h>

#include "ray.h"
#include "segment.h"
#include "triangle.h"
#include "plane.h"
#include "cylinder.h"
#include "aabb.h"
#include "obb.h"
#include "capsule.h"
#include "mesh_collider.h"

void Sphere::Init(Vector3 c, f32 r)
{
    this->c = c;
    this->r = r;
}

void Sphere::SetCenter(const Vector3& center)
{
    this->c = center;
}

Vector3 Sphere::GetCenter() const
{
    return c;
}

f32 Sphere::GetRadio() const
{
    return r;
}

bool Sphere::Intersect(const Sphere& sphere, Array<CollisionData>* collisionData) const
{
    Vector3 d = c - sphere.c;
    f32 dist2 = d.Dot(d);
    f32 radiusSum = r + sphere.r;
    return dist2 <= (radiusSum * radiusSum);
}

bool Sphere::Intersect(const Triangle& triangle, Array<CollisionData>* collisionData) const
{
    Vector3 closest = triangle.ClosestPoint(c);
    Vector3 toSphere = c - closest;
    f32 lenSq = toSphere.Dot(toSphere);
    bool isIntersecting = lenSq <= r * r;
    if (isIntersecting&& collisionData && collisionData->size < MAX_COLLISION_COUNT)
    {
        CollisionData collision;
        collision.n = toSphere.Normalized();
        collision.penetration = r - sqrtf(lenSq);
        collisionData->Push(collision);
    }
    return isIntersecting;
}

bool Sphere::Intersect(const Plane& plane, Array<CollisionData>* collisionData) const
{
    float dist = Vector3::Dot(c, plane.GetNormal()) - plane.GetDistance();
    bool isIntersecting = (dist - r) < 0.0f;
    if (isIntersecting && collisionData && collisionData->size < MAX_COLLISION_COUNT)
    {
        CollisionData collision;
        // TODO: ...
        collisionData->Push(collision);
    }
    return isIntersecting;
}

bool Sphere::Intersect(const AABB& aabb, Array<CollisionData>* collisionData) const
{
    return aabb.Intersect(*this, collisionData);
}

bool Sphere::Intersect(const OBB& obb, Array<CollisionData>* collisionData) const
{
    return obb.Intersect(*this, collisionData);
}

bool Sphere::Intersect(const Capsule& capsule, Array<CollisionData>* collisionData) const
{
    return capsule.Intersect(*this, collisionData);
}

bool Sphere::Intersect(const MeshCollider& meshCollider, Array<CollisionData>* collisionData) const
{
    return meshCollider.Intersect(*this, collisionData);
}

bool Sphere::DynamicIntersect(const Plane& plane, const Vector3& movement, f32& t) const
{
    Vector3 planeNormal = plane.GetNormal();
    // Compute distance of sphere center to plane
    f32 d = planeNormal.Dot(c);
    f32 dist = d - plane.GetDistance();
    if (fabsf(dist) <= r)
    {
        // The sphere is already overlapping the plane. Set time of
        // intersection to zero
        t = 0.0f;
        return true;
    }
    else
    {
        f32 denom = planeNormal.Dot(movement);
        if (denom * dist >= 0.0f)
        {
            // No intersection as sphere parallel to or away from plane
            return false;
        }
        else
        {
            // Sphere is moving towards the plane

            f32 radio = dist > 0.0f ? r : -r;
            t = (radio - dist) / denom;
            if (t < 0.0f || t > 1.0f)
            {
                return false;
            }
            return true;
        }
    }
}

bool Sphere::DynamicIntersect(const Triangle& triangle, const Vector3& movement, f32& t, Vector3& n) const
{
    Plane plane;
    plane.Init(triangle);

    if (DynamicIntersect(plane, movement, t))
    {

        Vector3 q = c + movement * t - plane.GetNormal() * r;
        if (triangle.PointInside(q))
        {
            n = triangle.n;
            return true;
        }
        else
        {
            Segment segment;
            segment.Init(c, c + movement);

            Cylinder cylinders[3];
            cylinders[0].Init(triangle.a, triangle.b, r);
            cylinders[1].Init(triangle.b, triangle.c, r);
            cylinders[2].Init(triangle.c, triangle.a, r);


            t = FLT_MAX;
            for (i32 i = 0; i < 3; i++)
            {
                f32 currentT;
                if (segment.Intersect(cylinders[i], currentT))
                {
                    if (currentT < t)
                    {
                        Segment segment;
                        segment.Init(cylinders[i].GetP(), cylinders[i].GetQ());

                        t = currentT;
                        Vector3 hitPos = c + movement * t;

                        f32 t0;
                        Vector3 closest = segment.ClosestPoint(hitPos, t0);

                        n = (hitPos - closest).Normalized();
                    }
                }
            }

            if (t != FLT_MAX)
            {
                return true;
            }
            else
            {
                Sphere spheres[3];
                spheres[0].Init(triangle.a, r);
                spheres[1].Init(triangle.b, r);
                spheres[2].Init(triangle.c, r);

                t = FLT_MAX;
                for (i32 i = 0; i < 3; i++)
                {
                    f32 currentT;
                    if (segment.Intersect(spheres[i], currentT))
                    {
                        if (currentT < t)
                        {
                            t = currentT;
                            Vector3 hitPos = c + movement * t;
                            n = (hitPos - spheres[i].c).Normalized();
                        }
                    }
                }
                return t != FLT_MAX;
            }
        }
    }
    else
    {
        Segment segment;
        segment.Init(c, c + movement);

        Cylinder cylinders[3];
        cylinders[0].Init(triangle.a, triangle.b, r);
        cylinders[1].Init(triangle.b, triangle.c, r);
        cylinders[2].Init(triangle.c, triangle.a, r);


        t = FLT_MAX;
        for (i32 i = 0; i < 3; i++)
        {
            f32 currentT;
            if (segment.Intersect(cylinders[i], currentT))
            {
                if (currentT < t)
                {
                    Segment segment;
                    segment.Init(cylinders[i].GetP(), cylinders[i].GetQ());

                    t = currentT;
                    Vector3 hitPos = c + movement * t;

                    f32 t0;
                    Vector3 closest = segment.ClosestPoint(hitPos, t0);

                    n = (hitPos - closest).Normalized();
                }
            }
        }

        if (t != FLT_MAX)
        {
            return true;
        }
        else
        {
            Sphere spheres[3];
            spheres[0].Init(triangle.a, r);
            spheres[1].Init(triangle.b, r);
            spheres[2].Init(triangle.c, r);

            t = FLT_MAX;
            for (i32 i = 0; i < 3; i++)
            {
                f32 currentT;
                if (segment.Intersect(spheres[i], currentT))
                {
                    if (currentT < t)
                    {
                        t = currentT;
                        Vector3 hitPos = c + movement * t;
                        n = (hitPos - spheres[i].c).Normalized();
                    }
                }
            }
            return t != FLT_MAX;
        }
    }

    return false;
}

Vector3 Sphere::ClosestPoint(const Vector3& point) const
{
    Vector3 delta = point - c;
    if (delta.MagnitudeSq() < r * r)
    {
        return point;
    }

    Vector3 dir = (point - c).Normalized();
    return c + dir * r;
}

float Sphere::SqDistPoint(const Vector3& point) const
{
    Vector3 closest = ClosestPoint(point);
    float sqDist = Vector3::Dot(closest - point, closest - point);
    return sqDist;
}