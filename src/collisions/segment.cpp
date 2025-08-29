#include "segment.h"
#include <math.h>

#include "plane.h"
#include "triangle.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"
#include "aabb.h"
#include "obb.h"
#include "mesh_collider.h"

#include <utils.h>

static const float EPSILON = 0.000001f;

void Segment::Init(Vector3 a, Vector3 b)
{
    this->a = a;
    this->b = b;
}

bool Segment::Intersect(const Triangle& triangle, f32& t) const
{
    Plane plane;
    plane.Init(triangle);

    if (plane.Intersect(*this, t))
    {
        Vector3 hitPos = a + (b - a).Normalized() * t;
        return triangle.PointInside(hitPos);
    }
    return false;
}

bool Segment::Intersect(const Sphere& sphere, f32& t) const
{
    Vector3 ab = b - a;
    Vector3 d = ab.Normalized();

    Vector3 m = a - sphere.GetCenter();
    f32 b_ = m.Dot(d);
    f32 c = m.Dot(m) - sphere.GetRadio() * sphere.GetRadio();

    if (c > 0.0f && b_ > 0.0f)
    {
        return false;
    }

    f32 discr = b_ * b_ - c;

    if (discr < 0.0f)
    {
        return false;
    }

    t = -b_ - sqrtf(discr);

    if (t*t > ab.MagnitudeSq())
    {
        return false;
    }

    t /= ab.Magnitude();
    if (t < 0.0f)
    {
        t = 0.0f;
    }

    return true;

}

bool Segment::Intersect(const Cylinder& cylinder, float& t) const
{
    Vector3 p = cylinder.GetP();
    Vector3 q = cylinder.GetQ();
    float r = cylinder.GetRadio();

    Vector3 d = q - p, m = a - p, n = b - a;
    f32 md = m.Dot(d);
    f32 nd = n.Dot(d);
    f32 dd = d.Dot(d);
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
            Vector3 x = a + n*t;
            return  t <= 1.0f && (x - p).MagnitudeSq() <= r * r;
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
            Vector3 x = a + n*t;
            return  t <= 1.0f && (x - q).MagnitudeSq() <= r * r;
        }
    }

    return t <= 1.0f;
}

bool Segment::Intersect(const Capsule& capsule, float& t) const
{
    Vector3 p = capsule.GetA();
    Vector3 q = capsule.GetB();
    float r = capsule.GetRadio();

    Vector3 d = q - p, m = a - p, n = b - a;
    f32 md = m.Dot(d);
    f32 nd = n.Dot(d);
    f32 dd = d.Dot(d);
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
        return t <= 1.0f;
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
        return Intersect(sphere, t) && t <= 1.0f;
    }

    if ((md + t * nd > dd))
    {
        Sphere sphere;
        sphere.Init(q, r);
        return Intersect(sphere, t) && t <= 1.0f;
    }

    return t >= 0.0f && t <= 1.0f;
}

bool Segment::Intersect(const Plane& plane, float& t) const
{
    Vector3 planeNormal = plane.GetNormal();
    t = (plane.GetDistance() - Vector3::Dot(planeNormal, a)) / Vector3::Dot(planeNormal, (b - a));
    return t >= 0.0f && t <= 1.0f;
}

bool Segment::Intersect(const AABB& aabb, float& t) const
{
    Vector3 min = aabb.GetMin();
    Vector3 max = aabb.GetMax();
    Vector3 ab = b - a;
    float length = ab.Magnitude();

    float tmin = 0.0f;
    float tmax = length;

    ab.Normalize();

    // For all three slabs
    for (int i = 0; i < 3; i++)
    {
        if (fabsf(ab[i]) < EPSILON)
        {
            // Ray is parallel to the slab, No hit if origin not within slab
            if (a[i] < min[i] || a[i] > max[i])
            {
                return false;
            }
        }
        else
        {
            // Compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / ab[i];
            float t1 = (min[i] - a[i]) * ood;
            float t2 = (max[i] - a[i]) * ood;
            // Make t1  be intersection with near plane. t2 with far plane
            if (t1 > t2) Utils::Swap(t1, t2);
            // Compute the intersection of slab intersection intervals
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    t = tmin / length;
    return t >= 0;
}

bool Segment::Intersect(const OBB& obb, float& t) const
{
    // transform the segment in the obb local space
    Vector3 x = obb.GetOrientation(0);
    Vector3 y = obb.GetOrientation(1);
    Vector3 z = obb.GetOrientation(2);

    Matrix4 invRotMat = Matrix4(
        x[0], x[1], x[2], 0,
        y[0], y[1], y[2], 0,
        z[0], z[1], z[2], 0,
        0, 0, 0, 1);

    Vector3 relA = Matrix4::TransformPoint(invRotMat, a - obb.GetCenter());
    Vector3 relB = Matrix4::TransformPoint(invRotMat, b - obb.GetCenter());

    Segment relSegment;
    relSegment.Init(relA, relB);

    AABB aabb;
    aabb.Init(obb.GetExtent() * -1.0f, obb.GetExtent());
    return relSegment.Intersect(aabb, t);
}

Vector3 Segment::ClosestPoint(const Vector3& point, f32& t) const
{
    Vector3 ab = b - a;
    t = (point - a).Dot(ab) / ab.Dot(ab);

    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return a + ab * t;
}

float Segment::ClosestPoint(const Segment& segment, Vector3& c1, float& s, Vector3& c2, float& t) const
{
    Vector3 d1 = b - a;
    Vector3 d2 = segment.b - segment.a;
    Vector3 r = a - segment.a;

    float a_ = d1.Dot(d1);
    float e = d2.Dot(d2);
    float f = d2.Dot(r);

    // Check if either or both segments degenerate into points
    if (a_ <= EPSILON && e <= EPSILON)
    {
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = a;
        c2 = segment.a;
        return (c2 - c1).MagnitudeSq();
    }

    if (a_ <= EPSILON)
    {
        // First segment degenerate into a point
        s = 0.0f;
        t = f / e;
        t = Utils::Clamp(t, 0, 1);
    }
    else
    {
        float c = d1.Dot(r);
        if (e <= EPSILON)
        {
            // Second segment degenerate into a point
            t = 0;
            s = Utils::Clamp(-c / a_, 0, 1);
        }
        else
        {
            // General non degenerate case start here
            float b_ = d1.Dot(d2);
            float denom = a_ * e - b_ * b_;
            // If segments not parallel, compute closest point on L1 to L2
            // and clamp to segment S1, Else pick arbitrary s (here 0)
            if (denom != 0.0f)
            {
                s = Utils::Clamp((b_ * f - c * e) / denom, 0, 1);
            }
            else
            {
                s = 0.0f;
            }
            // Compute the point on L2 Closest to S1
            t = (b_ * s + f) / e;

            if (t < 0.0f)
            {
                t = 0.0f;
                s = Utils::Clamp(-c / a_, 0, 1);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = Utils::Clamp((b_ - c) / a_, 0, 1);
            }
        }
    }

    c1 = a + d1 * s;
    c2 = segment.a + d2 * t;
    return (c2 - c1).MagnitudeSq();
}


float Segment::SqDistPoint(const Vector3& point) const
{
    Vector3 ab = b - a;
    Vector3 ac = point - a;
    Vector3 bc = point - b;
    float e = Vector3::Dot(ac, ab);
    // Handle case where c projects outside ab
    if (e <= 0.0f)
        return Vector3::Dot(ac, ac);
    float f = Vector3::Dot(ab, ab);
    if (e >= f)
        return Vector3::Dot(bc, bc);
    // Handle case where c projects onto ab
    return Vector3::Dot(ac, ac) - e * e / f;
}

Vector3 Segment::GetA() const
{
    return a;
}

Vector3 Segment::GetB() const
{
    return b;
}

Vector3 Segment::Lerp(float t) const
{
    return a + (b - a) * t;
}

