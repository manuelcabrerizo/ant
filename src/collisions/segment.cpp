#include "segment.h"
#include <math.h>

#include "plane.h"
#include "triangle.h"
#include "sphere.h"
#include "cylinder.h"

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
    Vector3 o = a;
    Vector3 d = ab.Normalized();

    Vector3 m = o - sphere.GetCenter();
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

    if (t * t > ab.Dot(ab))
    {
        return false;
    }

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    return true;

}

static const float EPSILON = 0.001f;

bool Segment::Intersect(const Cylinder& cylinder, f32& t) const
{
    Vector3 d = cylinder.q - cylinder.p, m = a - cylinder.p, n = b - a;
    f32 md = m.Dot(d);
    f32 nd = n.Dot(d);
    f32 dd = d.Dot(d);

    if (md < 0.0f && md + nd < 0.0f)
    {
        return false;
    }
    if (md > dd && md + nd > dd)
    {
        return false;
    }

    f32 nn = n.Dot(n);
    f32 mn = m.Dot(n);
    f32 a_ = dd * nn - nd * nd;
    f32 k = m.Dot(m) - cylinder.r * cylinder.r;
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

    if (t < 0.0f || t > 1.0f)
    {
        return false;
    }

    if (md + t * nd < 0.0f)
    {
        if (nd <= 0.0f)
        {
            return false;
        }
        t = -md / nd;
        return k + 2 * t * (mn + t * nn) <= 0.0f;
    }
    else if (md + t * nd > dd)
    {
        if (nd >= 0.0f)
        {
            return false;
        }
        t = (dd - md) / nd;
        return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
    }

    return true;
}


Vector3 Segment::ClosestPoint(const Vector3& point, f32& t) const
{
    Vector3 ab = b - a;
    t = (point - a).Dot(ab) / ab.Dot(ab);

    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return a + ab * t;
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
