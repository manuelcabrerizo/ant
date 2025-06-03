#include "ray.h"
#include <math.h>

#include "plane.h"
#include "triangle.h"
#include "sphere.h"

void Ray::Init(Vector3 o, Vector3 d)
{
    this->o = o;
    this->d = d;
}

bool Ray::Intersect(Triangle& triangle, f32& t)
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

bool Ray::Intersect(Sphere& sphere, f32& t)
{
    Vector3 m = o - sphere.c;
    f32 b = m.Dot(d);
    f32 c = m.Dot(m) - sphere.r * sphere.r;

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