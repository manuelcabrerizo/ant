#include "segment.h"
#include <math.h>

#include "plane.h"
#include "triangle.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"

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

    return t <= 1.0f;
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
