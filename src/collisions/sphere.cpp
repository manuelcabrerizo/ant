#include "sphere.h"
#include <math.h>
#include <float.h>

#include "ray.h"
#include "segment.h"
#include "triangle.h"
#include "plane.h"
#include "cylinder.h"

void Sphere::Init(Vector3 c, f32 r)
{
    this->c = c;
    this->r = r;
}

bool Sphere::Intersect(Ray& ray, f32& t)
{
    return ray.Intersect(*this, t);
}

bool Sphere::Intersect(Segment& segment, f32& t)
{
    return segment.Intersect(*this, t);
}

bool Sphere::Intersect(Triangle& triangle, Vector3& n, f32& penetration)
{
    Vector3 closest = triangle.ClosestPoint(c);

    //GraphicsManager::Get()->DebugDrawSphere(closest, 0.02f, 4, 6);

    Vector3 toSphere = c - closest;

    f32 lenSq = toSphere.Dot(toSphere);

    n = toSphere.Normalized();

    //GraphicsManager::Get()->DebugDrawLine(closest, closest + n);

    penetration = r - sqrtf(lenSq);
    if (lenSq <= r * r)
    {
        i32 StopHere = 0;
    }

    return lenSq <= r * r;
}

bool Sphere::DynamicIntersect(Plane& plane, Vector3 movement, f32& t)
{
    // Compute distance of sphere center to plane
    f32 d = plane.n.Dot(c);
    f32 dist = d - plane.d;
    if (fabsf(dist) <= r)
    {
        // The sphere is already overlapping the plane. Set time of
        // intersection to zero
        t = 0.0f;
        return true;
    }
    else
    {
        f32 denom = plane.n.Dot(movement);
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

bool Sphere::DynamicIntersect(Triangle& triangle, Vector3 movement, f32& u, f32& v, f32& w,
    f32& t, Vector3& n)
{
    Plane plane;
    plane.Init(triangle);

    if (DynamicIntersect(plane, movement, t))
    {

        Vector3 q = c + movement * t - plane.n * r;
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
                        segment.Init(cylinders[i].p, cylinders[i].q);

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
                // TODO: calculate barycentric
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
                    segment.Init(cylinders[i].p, cylinders[i].q);

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
            // TODO: calculate barycentric
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