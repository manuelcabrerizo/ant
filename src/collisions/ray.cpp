#include "ray.h"
#include <math.h>

#include "plane.h"
#include "triangle.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"
#include "aabb.h"
#include "obb.h"

#include <utils.h>
#include <cfloat>

static const float EPSILON = 0.000001f;

void Ray::Init(Vector3 o, Vector3 d)
{
    this->o = o;
    this->d = d;
}

bool Ray::Intersect(const Triangle& triangle, f32& t) const
{
    Plane plane;
    plane.Init(triangle);

    if (plane.Intersect(*this, t))
    {
        Vector3 hitPos = o + d * t;
        return triangle.PointInside(hitPos);
    }
    return false;
}

bool Ray::Intersect(const Sphere& sphere, f32& t) const
{
    Vector3 m = o - sphere.GetCenter();
    f32 b = m.Dot(d);
    f32 c = m.Dot(m) - sphere.GetRadio() * sphere.GetRadio();

    if (c > 0.0f && b > 0.0f)
    {
        return false;
    }

    f32 discr = b * b - c;

    if (discr < 0.0f)
    {
        return false;
    }

    t = -b - sqrtf(discr);

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    return true;
}

bool Ray::Intersect(const Cylinder& cylinder, float& t) const
{
    Vector3 p = cylinder.GetP();
    Vector3 q = cylinder.GetQ();
    float r = cylinder.GetRadio();

    Vector3 d_ = q - p, m = o - p, n = d;
    f32 md = m.Dot(d_);
    f32 nd = n.Dot(d_);
    f32 dd = d_.Dot(d_);
    f32 nn = n.Dot(n);
    f32 mn = m.Dot(n);

    f32 a_ = dd * nn - nd * nd;
    f32 k = m.Dot(m) - r * r;
    f32 c = dd * k - md * md;

    if (fabsf(a_) < EPSILON)
    {
        if (c > 0.0f)
        {
            return false;
        }

        if (md < 0.0f)
        {
            t = -mn / nn;
        }
        else if (md > dd)
        {
            t = (nd - mn) / nn;
        }
        else
        {
            t = 0.0f;
        }
        return true;
    }

    f32 b_ = dd * mn - nd * md;
    f32 discr = b_ * b_ - a_ * c;
    if (discr < 0.0f)
    {
        return false;
    }

    t = (-b_ - sqrtf(discr)) / a_;

    if ((md + t * nd) < 0)
    {
        if (nd <= 0.0f)
        {
            return false;
        }
        else
        {
            t = -md / nd;
            Vector3 x = o + n * t;
            return (x - p).MagnitudeSq() <= r * r;
        }
    }

    if ((md + t * nd > dd))
    {
        if (nd >= 0)
        {
            return false;
        }
        else
        {
            t = (dd - md) / nd;
            Vector3 x = o + n * t;
            return (x - q).MagnitudeSq() <= r * r;
        }
    }

    return true;
}

bool Ray::Intersect(const Capsule& capsule, float& t) const
{
    Vector3 p = capsule.GetA();
    Vector3 q = capsule.GetB();
    float r = capsule.GetRadio();

    Vector3 d_ = q - p, m = o - p, n = d;
    f32 md = m.Dot(d_);
    f32 nd = n.Dot(d_);
    f32 dd = d_.Dot(d_);
    f32 nn = n.Dot(n);
    f32 mn = m.Dot(n);

    f32 a_ = dd * nn - nd * nd;
    f32 k = m.Dot(m) - r * r;
    f32 c = dd * k - md * md;

    if (fabsf(a_) < EPSILON)
    {
        if (c > 0.0f)
        {
            return false;
        }

        if (md < 0.0f)
        {
            Sphere sphere;
            sphere.Init(p, r);
            Intersect(sphere, t);
        }
        else if (md > dd)
        {
            Sphere sphere;
            sphere.Init(q, r);
            Intersect(sphere, t);
        }
        else
        {
            t = 0.0f;
        }
        return true;
    }

    f32 b_ = dd * mn - nd * md;
    f32 discr = b_ * b_ - a_ * c;
    if (discr < 0.0f)
    {
        return false;
    }

    t = (-b_ - sqrtf(discr)) / a_;

    if ((md + t * nd) < 0)
    {
        Sphere sphere;
        sphere.Init(p, r);
        return Intersect(sphere, t);
    }

    if ((md + t * nd > dd))
    {
        Sphere sphere;
        sphere.Init(q, r);
        return Intersect(sphere, t);
    }

    return true;
}

bool Ray::Intersect(const Plane& plane, float& t) const
{
    t = (plane.d - Vector3::Dot(plane.n, o)) / Vector3::Dot(plane.n, d);
    return t >= 0.0f;
}

bool Ray::Intersect(const AABB& aabb, float& t) const
{
    Vector3 min = aabb.GetMin();
    Vector3 max = aabb.GetMax();

    float tmin = 0.0f;
    float tmax = FLT_MAX;

    // For all three slabs
    for (int i = 0; i < 3; i++)
    {
        if (fabsf(d[i]) < EPSILON)
        {
            // Ray is parallel to the slab, No hit if origin not within slab
            if (o[i] < min[i] || o[i] > max[i])
            {
                return false;
            }
        }
        else
        {
            // Compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / d[i];
            float t1 = (min[i] - o[i]) * ood;
            float t2 = (max[i] - o[i]) * ood;
            // Make t1  be intersection with near plane. t2 with far plane
            if (t1 > t2) Utils::Swap(t1, t2);
            // Compute the intersection of slab intersection intervals
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    t = tmin;
    return true;
}

bool Ray::Intersect(const OBB& obb, float& t) const
{
    // create a ray in the obb local space
    Vector3 x = obb.GetOrientation(0);
    Vector3 y = obb.GetOrientation(1);
    Vector3 z = obb.GetOrientation(2);

    Matrix4 invRotMat = Matrix4(
        x[0], y[0], z[0], 0,
        x[1], y[1], z[1], 0,
        x[2], y[2], z[2], 0,
        0, 0, 0, 1);

    invRotMat = Matrix4::Transposed(invRotMat);

    Vector3 relOrigin = Matrix4::TransformPoint(invRotMat, o - obb.GetCenter());
    Vector3 relDirection = Matrix4::TransformVector(invRotMat, d).Normalized();

    Ray relRay;
    relRay.Init(relOrigin, relDirection);

    AABB aabb;
    aabb.Init(obb.GetExtent() * -1.0f, obb.GetExtent());
    return relRay.Intersect(aabb, t);
}

Vector3 Ray::ClosestPoint(const Vector3& point, float& t) const
{
    t = (point - o).Dot(d) / d.Dot(d);
    if (t < 0) 
    {
        t = 0.0f;
    }
    return o + d * t;
}

float Ray::SqDistPoint(const Vector3& point) const
{
    float t;
    Vector3 closest = ClosestPoint(point, t);
    float sqDist = Vector3::Dot(closest - point, closest - point);
    return sqDist;
}