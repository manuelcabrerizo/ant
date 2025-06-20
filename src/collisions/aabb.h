#pragma once

#include <common.h>
#include <containers.h>
#include <math/vector3.h>

class Plane;
class Sphere;

class AABB
{
private:
    Vector3 min;
    Vector3 max;
public:
    void Init(const Vector3& min, const Vector3& max);
    void Init(Array<Vector3>& points);

    Vector3 GetMin() const;
    Vector3 GetMax() const;

    bool Intersect(const AABB& aabb) const;
    bool Intersect(const Plane& plane) const;
    bool Intersect(const Sphere& sphere) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;
};