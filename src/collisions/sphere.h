#pragma once

#include <common.h>
#include <math/vector3.h>

class Triangle;
class Ray;
class Segment;
class Plane;

class Sphere
{
private:
    Vector3 c;
    f32 r;
public:
    void Init(Vector3 c, f32 r);

    Vector3 GetCenter() const;
    f32 GetRadio() const;

    bool Intersect(const Sphere& sphere) const;
    bool Intersect(const Ray& ray, f32& t) const;
    bool Intersect(const Segment& segment, f32& t) const;
    bool Intersect(const Triangle& triangle, Vector3& n, f32& penetration) const;
    bool Intersect(const Plane& plane);

    bool DynamicIntersect(const Plane& plane, const Vector3& movement, f32& t) const;
    bool DynamicIntersect(const Triangle& triangle, const Vector3& movement, f32& t, Vector3& n) const;

    Vector3 ClosestPoint(const Vector3& point) const;
};