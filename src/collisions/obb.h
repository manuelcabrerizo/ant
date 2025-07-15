#pragma once

#include <common.h>
#include <math/vector3.h>

class Plane;
class Sphere;

class OBB
{
private:
    Vector3 c;
    Vector3 u[3];
    Vector3 e;
public:
    void Init(const Vector3& center, const Vector3 orientation[], const Vector3& extent);

    Vector3 GetCenter() const;
    Vector3 GetOrientation(int axis) const;
    Vector3 GetExtent() const;

    bool Intersect(const OBB& obb) const;
    bool Intersect(const Plane& plane) const;
    bool Intersect(const Sphere& sphere) const;


    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(const Vector3& color);
};