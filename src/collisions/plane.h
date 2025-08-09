#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;

class Plane
{
private:
    Vector3 n;
    f32 d;
public:

    void Init(const Vector3& n, f32 d);
    void Init(const Vector3& point, const Vector3 n);
    void Init(const Triangle& triangle);

    bool Intersect(const Ray& ray, f32& t) const;
    bool Intersect(const Segment& segment, f32& t) const;
    bool Intersect(const Vector3& point) const;

    Vector3 ClosestPoint(const Vector3& q) const;
    float DistPoint(const Vector3& q) const;

    Vector3 GetNormal() const;
    float GetDistance() const;
};
